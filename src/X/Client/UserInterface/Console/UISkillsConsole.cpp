#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Console/TouchGestureTracker.h"
#include "ConsoleInputUtilities.h"
#include "Wrappers/RadioSelectorWrapper.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIWrapPanelControl.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/DataStructures/XBitArray.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/UnicodeText.h"

#include <stdlib.h>
#include <string.h>

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Player.h:803. Only the members this file reads are declared.
struct Player {  // size 0xd770
    int32 nPlayerIndex;  // 0x00
    ACDID idPlayerACD;  // 0x04
    uint8 _unadopted0[0x1348 - 0x8];  // 0x08 (invented name: .. tHeroStateData.tSavedData.tActiveSkills)
    struct {
        SNO snoSkill;  // 0x00
        RuneType eRuneType;  // 0x04
        uint8 _unadopted[0x8];  // 0x08
    } tActiveSkills[6];  // 0x1348 (tHeroStateData.tSavedData.tActiveSkills)
    SNO snoTraits[4];  // 0x13a8 (tHeroStateData.tSavedData.snoTraits)
    GBID gbidLegendaryPowers[3];  // 0x13b8 (tHeroStateData.tSavedData.gbidLegendaryPowers)
    uint8 _unadopted1[0xc224 - 0x13c4];  // 0x13c4 (invented name: .. nCurrentLevel)
    int32 nCurrentLevel;  // 0xc224
    uint8 _unadopted2[0xc6ec - 0xc228];  // 0xc228 (invented name: .. nControllerIndex)
    int32 nControllerIndex;  // 0xc6ec
    uint8 _unadopted3[0xd770 - 0xc6f0];  // 0xc6f0
};
static_assert(sizeof(Player) == 0xd770, "Player");
typedef int32 LegendaryPowerSlotType;
// X/Common/Player.cpp
extern const int32 cgnActiveSkillSlotLevelReq[6];
extern const int32 cgnTraitSlotLevelReq[4];
int32 PlayerIndexToLocalPlayerIndex(int32 nPlayerIndex);
PlayerClass PlayerGetPlayerClass(ACDID idPlayer);
PlayerClass PlayerGetPlayerClass(const ActorCommonData * ptACD);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
BOOL PlayerCanChangeSkillorTrait(const ActorCommonData & tACDPlayer, BOOL bIsTrait);
BOOL PlayerCanOpenInventory(const ActorCommonData & tACDPlayer);
CRefString PlayerGetSpellRuneToolTip(const ActorCommonData & tACD, SNO snoPower, RuneType eRune, BOOL bShowDetails, BOOL bUnused, BOOL & fFoundCombinedTooltip);
// X/Common/Crafting/ItemCrafting.cpp
int32 GetCraftLevel(int32 eCrafterType, const Player & tPlayer);
// X/Client/Definitions/PreferencesDefinition.h:286
class GameplayPrefs {  // size 0x70
public:
    BOOL GetSkillElectiveMode() const { return bElectiveMode; }  // line 542
    BOOL ShowAdvancedTooltips() const;
    uint8 _unadopted0[0x44];  // 0x00 (invented name: nItemsOnGroundSetting .. bShowAdvancedTooltips)
    BOOL bShowAdvancedTooltips;  // 0x44
    BOOL bElectiveMode;  // 0x48
    uint8 _unadopted1[0x70 - 0x4c];  // 0x4c
};
namespace Prefs {
const GameplayPrefs & GetGameplayPrefs();
}  // namespace Prefs
// X/Common/Game.cpp
BOOL GameIsWeeklyChallenge();
// X/Common/ChallengeRift.cpp
namespace ChallengeRift {
RuneType GetRuneTypeForSkill(SNO snoPower);
BOOL IsValidChallengeRiftSkill(SNO snoPower);
}  // namespace ChallengeRift
// X/Common/GlobalSNO.cpp
typedef int32 GlobalGBID;
GBID GlobalGBIDGet(const GlobalGBID eGlobalGBID);
// X/Common/TagMaps/PowerTags.h
enum {
    TAG_POWER_ITEM_TYPE_REQUIREMENT = 328960,
};
// X/Common/Powers/PowersCommon.cpp
BOOL PowerIsDisplayed(AutoPowerDefinitionR pdefPower);
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);
CRefString PowerGetNameLabel(SNO snoPower);
CRefString PowerGetRuneName(SNO snoPower, RuneType eRune);
RuneType PowerGetRuneTypeForACD(const ActorCommonData & tACD, SNO snoPower);
GBID PowerGeneralTagGetGBID(AutoPowerDefinitionR pdefPower, uint32 dwTag);
BOOL PowerActorMeetsWeaponRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
// X/Common/Attributes/FastAttribute.cpp
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);
// X/Common/Items/ItemTypes.cpp
CRefString ItemTypeGetDescription(GBID gbidItemType, BOOL bPlural, BOOL bUnused);
// X/Common/GameBalance.cpp
CRefString GBIDToString(const GBID & gbid);
// X/Sigma/BitManip.cpp
void SetBit(uint8 * pbBits, int32 nBit);
BOOL TestBit(const uint8 * pbBits, int32 nBit);
#include "../../../Common/Shell/CmdLine.h"






const CmdLineParams * CmdLineGetParams();
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
CRefString StringListEntryReplace(LPCSTR szString, LPCSTR szToken, LPCSTR szReplacement, BOOL bReplaceAll);
// X/Client/UserInterface/Controls/UITextControl.cpp
void Set_Text_Color(CRefString & sText, int32 eColor);
// X/Client/Input/Input.h:314
struct TouchState {  // size 0x8
    uint16 uX;  // 0x00
    uint16 uY;  // 0x02
    uint32 uID;  // 0x04
};
struct ControllerFrameState {  // size 0x38
    uint8 _unadopted0[0x20];  // 0x00 (invented name: bIsConnected .. vecRightStick)
    int32 nNumTouches;  // 0x20
    TouchState arTouches[2];  // 0x24
    uint8 _unadopted1[0x38 - 0x34];  // 0x34 (invented name: tGestures .. tGestures)
};
struct ControllerState {  // size 0x10c
    ControllerFrameState tPrevFrame;  // 0x00
    ControllerFrameState tCurrentFrame;  // 0x38
    uint8 _unadopted0[0x10c - 0x70];  // 0x70 (invented name: tButtonLastDownTimes .. tRStickLastExtremeTime)
};
struct InputState {  // size 0x760
    uint8 _unadopted0[0x330];  // 0x00 (invented name: dwFlags .. tMouseState)
    ControllerState tControllerState[4];  // 0x330
};
static_assert(sizeof(InputState) == 0x760, "InputState");
enum InputContext {
    INPUTCONTEXT_DEFAULT = 0,
};
const InputState & InputGetCurrentState(InputContext eContext);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
UIWrapPanel & UI_GetWrapPanelFromID(const UIID & id);
BOOL UI_TestID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer);
BOOL ShowTieredRiftMessage(const Player * ptPlayer);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
SkillCategory GetSkillCategoryForSlot(const int32 nSlot, const ACDID idPlayer);
CRefString GetSkillCategoryLabel(const SkillCategory eCategory);
CRefString GetSkillDescriptionText(ActorCommonData & tACD, SNO snoPower, int32 nSkillLevel, BOOL bIsTrait, BOOL bShowDetails);
BOOL GetName(SNO snoPower, CRefString & sName);
}  // namespace UISkills
// X/Client/UserInterface/Interface/UIEquipLegendaryPowers.cpp
namespace UIEquipLegendaryPowers {
CRefString GetPowerTooltipText(GBID gbidPower, bool bUnused);
void ShowPowerIcon(const UIID & idVisual, GBID gbidPower, BOOL bUnused);
}  // namespace UIEquipLegendaryPowers
// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
BOOL IsButtonDown(int32 nControllerIndex, int32 eButton);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bUnused);
}  // namespace ControllerUtils
}  // namespace Console
#include "ControllerActions.h"
namespace Console {
typedef int32 ControllerInput;
typedef int32 ControllerActionId;








}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
void UpdateButtonLegendText();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_SKILLS_CONSOLE;
extern UIID UIWND_SKILLS_CONSOLE_RING;
extern UIID UIWND_SKILLS_CONSOLE_RING_CURSOR;
extern UIID UIWND_SKILLS_CONSOLE_RING_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_PASSIVE_RING_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_MENU_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_ACTIVE_SKILLS;
extern UIID UIWND_SKILLS_CONSOLE_ACTIVE_SKILLS_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_RUNES;
extern UIID UIWND_SKILLS_CONSOLE_RUNES_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS;
extern UIID UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS_GLOW;
extern UIID UIWND_SKILLS_CONSOLE_KANAIS_POWERS;
extern UIID UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON;
extern UIID UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_LOCKED;
extern UIID UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_UNASSIGNED;
extern UIID UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_ASSIGNED;
extern UIID UIWND_SKILLS_CONSOLE_ELECTIVE_MODE;
extern UIID UIWND_SKILLS_CONSOLE_SKILL_INFO_DIMMER;
extern UIID UIWND_SKILLS_CONSOLE_RUNE_INFO_DIMMER;
extern UIID UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO_DIMMER;
extern UIID UISTACKPANEL_SKILLS_CONSOLE_ACTIVE_SKILLS;
extern UIID UISTACKPANEL_SKILLS_CONSOLE_RUNES;
extern UIID UIWRAPPANEL_SKILLS_CONSOLE_PASSIVE_SKILLS;
extern UIID UISCROLL_SKILLS_CONSOLE_SKILL_INFO;
extern UIID UISCROLL_SKILLS_CONSOLE_RUNE_INFO;
extern UIID UISCROLL_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO;
extern UIID UITXT_SKILLS_CONSOLE_HEADER;
extern UIID UITXT_SKILLS_CONSOLE_CATEGORY_NAME;
extern UIID UITXT_SKILLS_CONSOLE_CATEGORY_DESCRIPTION;
extern UIID UITXT_SKILLS_CONSOLE_SKILL_NAME;
extern UIID UITXT_SKILLS_CONSOLE_SKILL_INFO;
extern UIID UITXT_SKILLS_CONSOLE_RUNE_NAME;
extern UIID UITXT_SKILLS_CONSOLE_RUNE_INFO;
extern UIID UITXT_SKILLS_CONSOLE_PASSIVE_SKILLS_NAME;
extern UIID UITXT_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO;

// AppGlobals (X/Common/Game.h:940) comes from X/Common/Game.h via ActorCommonData.h.




extern AppGlobals sgtAppGlobals;
// X/Common/Player.cpp
Player * PlayerGetByACD(ACDID idACD);
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
uint32 PlayerGetLocalPlayerCount();
BOOL PlayerIsLocal(const Player * ptPlayer);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
GBID PlayerClassToGBID(PlayerClass ePlayerClass);
// X/Common/Player.h:280
typedef int32 GameError;
enum {
    GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COOLDOWN = 54,
    GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_BOSS_ENCOUNTER = 56,
};
struct GameErrorData {  // size 0x8
    GameErrorData(const GameError eError) : eError(eError), dwVal(0) {}  // line 318
    GameError eError;  // 0x00
    uint32 dwVal;  // 0x04
};
// X/Client/CPlayer.cpp
ACDID LocalPlayersACDGetByIndex(int32 nIndex);
CRefString CPlayerGetIndicatorString(const Player & tPlayer);
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
}  // namespace UIGame
// X/Common/VisualInventory.cpp
struct VisualInventory;
VisualInventory * VisualInventoryGet(ACDID idACD);
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
namespace UIInventory {
namespace Console {
void UpdatePlayerActorUI(const UIID & idRActor, VisualInventory & tVisualInventory, const Player & tPlayer, uint32 dwFlags);
void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idParent, LPCSTR szNavBar, int32 nTab);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleCharacterDetails.cpp
namespace UICharacterDetails {
namespace Console {
void SetReturnScreen(int32 nScreen);
}  // namespace Console
}  // namespace UICharacterDetails
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/Interface/UITutorials.h:34
#include "../Interface/UITutorials.h"



extern UITutorialManager gtTutorialMgr;
// X/Client/Console/Controller.cpp
namespace Controller {
CRefString GetActionButtonIconName(int32 nSlot, const Player * ptPlayer, BOOL bUnused);
}  // namespace Controller
// X/Common/GameBalance.cpp
struct GameBalanceDefinition;



const void * GBLockR(const GBHandle & hGB, const GameBalanceDefinition *& pdefGB, const BOOL & bUnused);
void GBUnlockR(const GameBalanceDefinition *& pdefGB);
#include "../../../Common/Definitions/GameBalanceDefinition.h"





// X/Common/Definitions/ActorDefinition.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
#include "../../../Common/Definitions/HeroDefinition.h"















#include "../../../Common/Networking/GameMessage.h"





// X/Client/UserInterface/Console/ConsoleUIRActorManager.h
namespace Console {
namespace UIRActorManager {
enum {
    RACTOR_CONTROL_RING = 1,
};
BOOL ShouldCreateRActor(int32 eRActorControl, int32 eRActorCharacterType, BOOL bForceRecreate, BOOL bClean);
void ForceRActorRecreate();
void SetActive(BOOL bActive);
}  // namespace UIRActorManager
namespace Screens {
BOOL IsScreenSetOpen();
void OpenToScreen(int32 nScreenSet, int32 nScreen, int32 nControllerIndex, BOOL bUnused);
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
extern UIID UIRACTOR_CONSOLE_RING;
extern UIID UITXT_CONSOLE_PLAYER_LABEL_RING;
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
SNO GetObserverForPlayerClass(PlayerClass ePlayerClass);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
int32 SkillCategoryGetSlot(const SkillCategory eCategory);
}  // namespace UISkills

int32 s_nTouchDistanceThreshold = 210;  // line 64

namespace UISkills {
namespace Console {
enum AssignmentType {
    NOT_ASSIGNED = 0,
    ASSIGNED_IN_SLOT = 1,
    ASSIGNED_IN_ANOTHER_SLOT = 2,
};

struct RuneEntry {  // size 0x8
    RuneType eRuneType;  // 0x00
    int32 nLevel;  // 0x04
    RuneEntry() : eRuneType(-1), nLevel(0) {}  // line 239
};
static_assert(sizeof(RuneEntry) == 0x8, "RuneEntry");

struct SkillListEntry {  // size 0x38
    UISkills::Console::RuneEntry tRunes[5];  // 0x00
    int32 nIndex;  // 0x28
    int32 nLevel;  // 0x2c
    SNO snoPower;  // 0x30
    SkillCategory eCategory;  // 0x34
    SkillListEntry() : nIndex(-1), nLevel(0), snoPower(-1), eCategory(-1) {}  // line 256
};
static_assert(sizeof(SkillListEntry) == 0x38, "SkillListEntry");

typedef blz::vector<UISkills::Console::SkillListEntry, blz::allocator<UISkills::Console::SkillListEntry> > SkillList;

struct ExploredSkillInfo {  // size 0x28
    uint8 tExploredTraits[3];  // 0x00
    uint8 tExploredSkills[27];  // 0x03
    BOOL bHasNewSkills;  // 0x20
    BOOL bNewSkillKit;  // 0x24
    ExploredSkillInfo() {  // line 299
        SigmaMemoryClear(tExploredSkills, sizeof(tExploredSkills));
        SigmaMemoryClear(tExploredTraits, sizeof(tExploredTraits));
        bHasNewSkills = FALSE;
        bNewSkillKit = FALSE;
    }
};
static_assert(sizeof(ExploredSkillInfo) == 0x28, "ExploredSkillInfo");

enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_CHOOSE_SKILL = 4,
    ACTION_ASSIGN_SKILL = 5,
    ACTION_ASSIGN_RUNE = 6,
    ACTION_ASSIGN_PASSIVE = 7,
    ACTION_CLEAR_SKILL = 8,
    ACTION_CLOSE = 9,
    ACTION_SHOW_CHARACTER_DETAILS = 10,
    ACTION_SCROLL_DETAIL_INFO = 11,
};

class PlayerSkillsGlobals {  // size 0x720
public:
    enum SelectionMode {
        SLOT_SELECTION_MODE = 0,
        SKILL_SELECTION_MODE = 1,
        RUNE_SELECTION_MODE = 2,
        TRAIT_SELECTION_MODE = 3,
    };
    const Player * ptPlayer;  // 0x00
    UISkills::Console::SkillList listSkillsInSelectedSlot;  // 0x08
    BOOL bShowDetailedInfo;  // 0x20
    BOOL m_fCurrentlyOpening;  // 0x24
    UISkills::Console::ExploredSkillInfo arExploredSkills[4];  // 0x28
    UISkills::Console::ExploredSkillInfo arSnapshotSkills[4];  // 0xc8
    ::Console::RingWrapper tRingUI;  // 0x168
    ::Console::AnalogStickInputFilter tRingStickInputFilter;  // 0x1d0
    PlayerSkillsGlobals();
    ~PlayerSkillsGlobals();
    void OnOpen(const Player * _ptPlayer);
    void OnUpdate();
    void Refresh();
    BOOL IsActive() const;
    void OpenSelectedSlot();
    void CloseSelectedSlot();
    void GoBack();
    int32 GetSelectedSlot() const;
    const UISkills::Console::SkillListEntry * GetSelectedSkill() const;
    const UISkills::Console::RuneEntry * GetSelectedRune() const;
    const UISkills::Console::SkillListEntry * GetSelectedTrait() const;
    UISkills::Console::PlayerSkillsGlobals::SelectionMode GetSelectionMode() const;
    void SetSelectionMode(const UISkills::Console::PlayerSkillsGlobals::SelectionMode eMode);
    void UpdateButtonLegend();
    void UpdateScrollBoxes();
    void AssignSelectedSkill();
    void AssignSelectedRune();
    void AssignSelectedTrait();
    void HandleStickForWrapPanel(const ::Console::AnalogStickInputFilter::Rule::ID);
    void OnLeftStick(const UIID & id, const int32 nControllerIndex);
    void OnButton4(const UIID &, const int32);
    void OnButtonL2(const UIID &, const int32);
    void OnButtonR2(const UIID &, const int32);
    void OnDPadLeft(const int32);
    void OnDPadRight(const int32);
    void OnDPadUp(const int32);
    void OnDPadDown(const int32);
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    BOOL SkillSelectionPredicate(const int32 nIndex);
    void OnSkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
    BOOL RuneSelectionPredicate(const int32 nIndex);
    void OnRuneSelectionChanged(const UIID & idRune, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
    BOOL TraitSelectionPredicate(const int32 nIndex);
    void OnTraitSelectionChanged(const UIID & idTrait, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
    void CacheRingSelection();
    BOOL RestoreCachedRingSelection();
    void StopAllStoryboards();
private:
    UISkills::Console::PlayerSkillsGlobals::SelectionMode m_eSelectionMode;  // 0x1f8
    ::Console::RadioSelectorWrapper m_tSkillSelector;  // 0x200
    ::Console::RadioSelectorWrapper m_tRuneSelector;  // 0x238
    ::Console::RadioSelectorWrapper m_tTraitSelector;  // 0x270
    UISkills::Console::SkillList m_tSkills[42];  // 0x2a8
    UISkills::Console::SkillList m_tTraits;  // 0x698
    int32 m_nSelectedSkillIndex;  // 0x6b0
    int32 m_nSelectedRuneIndex;  // 0x6b4
    int32 m_nSelectedTraitIndex;  // 0x6b8
    int32 m_nCachedRingSelection;  // 0x6bc
    int32 m_nCurrentlyViewedSkillSlot;  // 0x6c0
    ::Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x6c8
    ::Console::TouchGestureTracker m_tTouchInputHandler;  // 0x6f0
    void RefreshSkillsList();
    int32 GetNumSkills(const int32 nSlot) const;
    const UISkills::Console::SkillListEntry & GetSkill(const int32 nSlot, const int32 nIndex) const;
    const UISkills::Console::SkillListEntry * GetSkillWithSNO(const int32 nSlot, const SNO snoPower) const;
    int32 GetIndexOfSkill(const int32 nSlot, const SNO snoPower) const;
    const UISkills::Console::SkillListEntry * GetSkillForVisibleRunes() const;
    int32 GetNumRunes() const;
    void RefreshRunesList();
    const UISkills::Console::RuneEntry * GetRune(const int32 nIndex) const;
    int32 GetIndexOfRune(const RuneType eRuneType) const;
    int32 GetNumTraits() const;
    void RefreshTraitsList();
    const UISkills::Console::SkillListEntry & GetTrait(const int32 nIndex) const;
    int32 GetIndexOfTrait(const SNO snoPower) const;
    void EnableRingUI(const BOOL bEnable);
    ::Console::RadioSelectorWrapper * GetFocusedRadioSelector();
public:
    enum SelectorDirection {
        DIRECTION_LEFT = 0,
        DIRECTION_RIGHT = 1,
        DIRECTION_UP = 2,
        DIRECTION_DOWN = 3,
    };
private:
    void MoveTraitSelector(const UISkills::Console::PlayerSkillsGlobals::SelectorDirection eDirection);
    void MoveSkillSelector(const UISkills::Console::PlayerSkillsGlobals::SelectorDirection eDirection);
    void SendAssignSkillMessage(const BOOL bAssignRune);
    BOOL SendAssignTraitMessage();
    void AnimateSelection();
    void AnimateHighlight();
    void AnimateLoadoutEquip();
    void RefreshUI();
    void UpdateTouchInput();
    void UpdateHeaderText();
    void UpdateCategoryText();
    void UpdateFocusGlows();
    void UpdateRuneVisibility();
    void UpdateSkillAndRuneUIs();
    void UpdateKanaisPowersUI();
    void UpdateSkillsListUI();
    void UpdateSkillItemUI(const int32 nIndex, const BOOL bIsTrait);
    void UpdateSkillInfoUI();
    void UpdateRunesListUI();
    void UpdateRunesListItemUI(const int32 nIndex);
    void UpdateRuneInfoUI();
    void UpdateTraitsListUI();
    void UpdateTraitInfoUI();
    BOOL CanAssignSkill(const UISkills::Console::SkillListEntry * ptSkill) const;
    BOOL CanAssignTrait(const UISkills::Console::SkillListEntry * ptSkill) const;
    BOOL CanAssignRune(const UISkills::Console::RuneEntry * ptRune) const;
    BOOL IsInTieredRift() const;
    void InitRingUI();
    void InitAnalogStickRules();
    void UpdateSkillRing();
    void UpdateSkillSlotIcon(const int32 nSlot);
    void UpdateTraitSlotIcon(const int32 nSlot);
    void UpdateKanaisPowersSlotIcon();
};
static_assert(sizeof(PlayerSkillsGlobals) == 0x720, "PlayerSkillsGlobals");

class CategorySkillFilter {  // size 0x8
public:
    static const int32 MAX_SKILLS = 5;
    CategorySkillFilter(const SkillCategory eCategory, const PlayerClass ePlayerClass)  // line 4385
        : m_eCategory(eCategory), m_ePlayerClass(ePlayerClass) {}
    BOOL IncludeSkill(const ActiveSkillEntry & tSkill) const;
private:
    SkillCategory m_eCategory;  // 0x00
    PlayerClass m_ePlayerClass;  // 0x04
};
static_assert(sizeof(CategorySkillFilter) == 0x8, "CategorySkillFilter");

class ElectiveSkillFilter {  // size 0x4
public:
    static const int32 MAX_SKILLS = 30;
    ElectiveSkillFilter(const PlayerClass ePlayerClass) : m_ePlayerClass(ePlayerClass) {}  // line 4410
    BOOL IncludeSkill(const ActiveSkillEntry &) const { return TRUE; }
private:
    PlayerClass m_ePlayerClass;  // 0x00
};
static_assert(sizeof(ElectiveSkillFilter) == 0x4, "ElectiveSkillFilter");

static const int32 MAX_SKILLS_PER_CATEGORY_CONSOLE = 5;  // line 70
static const int32 MAX_RUNE_RANKS_CONSOLE = 5;  // line 71
static const int32 KANAI_SLOT = 10;  // line 72

// Used before they are defined.
BOOL sSkillSelectionPredicate(const int32 nIndex);
void sSkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
BOOL sRuneSelectionPredicate(const int32 nIndex);
void sRuneSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
BOOL sTraitSelectionPredicate(const int32 nIndex);
void sTraitSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
BOOL SlotIsSkill(const int32 nSlot);
BOOL SlotIsPassive(const int32 nSlot);
int32 SlotFromIcon(const UIID & idIcon);
int32 GetTraitSlotNumber(const int32 nSlot);
void ShowCategoryName(const UIID & id, const int32 nSlot, const SkillCategory eCategory);
void ShowCategoryDescription(const UIID & id, const Player * ptPlayer, const int32 nSlot, const SkillCategory eCategory);
SkillCategory GetSkillCategory(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait);
AssignmentType IsSkillAssigned(const Player & tPlayer, const int32 nSlot, const SNO snoPower, const BOOL bIsTrait);
BOOL ShouldShowSkillAsNew(const Player & tPlayer, const int32 nSlot, const SkillListEntry & tSkillEntry, const BOOL bIsTrait);
BOOL HasNewSkillOrRune(const int32 nSlot, const SNO snoPower);
CRefString GetSkillName(const SNO snoPower);
void ShowSkillDescription(const UIID & idText, const SNO snoPower, ActorCommonData & tACDPlayer, const BOOL bIsTrait, const BOOL bShowDetails);
CRefString GetRuneTextureFrame(const RuneType eRuneType, const BOOL bEquipped);
void ShowRuneDescription(const UIID & idText, const SNO snoPower, const RuneType eRuneType, const BOOL bShowDetails);
BOOL IsWaitingForCooldown(const int32 nSlot);
BOOL PlayerMeetsSkillLevelRequirement(const ACDID idPlayer, const SkillListEntry * ptSkill);
BOOL PlayerMeetsRuneLevelRequirement(const ACDID idPlayer, const RuneEntry * ptRune);
BOOL IsSlotUnlocked(const ActorCommonData & tACDPlayer, const int32 nSlot);
void UpdateLockedIndicator(const UIID & idParent, const BOOL bIsLocked);
void UpdateAssignedIndicator(const UIID & idParent, const AssignmentType eAssignment);
void UpdateSelectionHighlight(const UIID & idParent, const BOOL bIsSelected);
void UpdateNewItemIndicator(const UIID & idParent, const BOOL bIsNew);
int32 SkillRuneIndexToBit(const int32 nSkillIndex, const RuneType eRune);
BOOL EntryHasNewRune(const SkillListEntry & tCachedListEntry, const RuneEntry & tRuneEntry, const int32 nLocalPlayerIndex, const int32 nLevel);
BOOL SkillHasNewRune(const SkillListEntry & tCachedListEntry, const int32 nLevel);
BOOL sGetSkillListEntry(const SNO snoPower, const PlayerClass ePlayerClass, const BOOL bIsTrait, SkillListEntry & tSkillListEntry);
int32 GetRuneLevel(const SNO snoPower, const RuneType eRune, const ActorCommonData & tACDPlayer);
BOOL IsTraitNew(const SkillListEntry & tCachedListEntry, const int32 nLocalPlayerIndex, const int32 nLevel);
BOOL IsSkillNew(const SkillListEntry & tCachedListEntry);
BOOL HasNewTrait(const int32 nLevel);
void GetSkillListForSlot(const SkillKitDefinition * pdefSkillKit, const ACDID idPlayer, SkillList & listSkills, int32 nSlot);
void GetElectiveSkillList(const SkillKitDefinition * pdefSkillKit, const PlayerClass ePlayerClass, SkillList & listSkills);
void GetTraitList(const SkillKitDefinition * pdefSkillKit, const PlayerClass ePlayerClass, SkillList & listTraits);
UIID GetUIIDForSkillSlot(const int32 nSlot, const UIID & idRing);
UIID GetUIIDForTraitSlot(const int32 nSlot, const UIID & idRing);
void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);
BOOL IconIsKanaiPowers(const UIID & idIcon);
BOOL IconIsSkill(const UIID & idIcon);
void AnimateSkillHighlight(const UIID & idItem);
void AnimateSkillSelection(const UIID & idItem);
void AnimateEquippedSkillIcon(const UIID & idSlot);
void sStoryboardEndCallback(const UIID & idCaller);
void PlaySelectionSound(const UIID & idParent, const BOOL bSelected);
void UpdateCharacterInfo(const Player & tPlayer, BOOL bForceUpdate);
void SetUp3DCharacter(const Player & tPlayer);
void InitRingSelection();
void Refresh(const Player * ptPlayer);
BOOL IsActive();
LPCSTR GetRomanNumeralForTraitSlot(const int32 nSlot);
int32 SortRunes(const void * arg0, const void * arg1);
int32 SortSkills(const void * arg0, const void * arg1);
void UpdateSkillSlotIcon(const UIID & idSkillIcon, const int32 nSlot, const Player * ptPlayer, const SNO snoPower, const BOOL fRingEnabled, const BOOL fSlotSelected, const BOOL fOnCooldown, const BOOL fForInspect);
void UpdateTraitSlotIcon(const UIID & idTraitSlot, const int32 nSlot, const Player * ptPlayer, const SNO snoPower, const BOOL fRingEnabled, const BOOL fSlotSelected, const BOOL fOnCooldown, const BOOL fForInspect);
int32 GetSkillLevel(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait);
BOOL sGetSkillListEntry(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait, SkillListEntry & tSkillListEntry);
SkillCategory GetSkillCategory(const SNO snoPower, const PlayerClass ePlayerClass, const BOOL bIsTrait);

SNO GetSkillForSlot(const Player & tPlayer, const int32 nSlot)  // line 215
{
    if (nSlot == -1) {
        return -1;
    }
    return tPlayer.tActiveSkills[nSlot].snoSkill;
}

SNO GetTraitForSlot(const Player & tPlayer, const int32 nSlot)  // line 225
{
    if (nSlot == -1) {
        return -1;
    }
    return tPlayer.snoTraits[nSlot];
}

PlayerSkillsGlobals::~PlayerSkillsGlobals()  // line 332
{
}

PlayerSkillsGlobals::PlayerSkillsGlobals()  // line 357
    : ptPlayer(NULL),
      bShowDetailedInfo(FALSE),
      m_fCurrentlyOpening(FALSE),
      m_eSelectionMode(SLOT_SELECTION_MODE),
      m_nSelectedSkillIndex(-1),
      m_nSelectedRuneIndex(-1),
      m_nSelectedTraitIndex(-1),
      m_nCachedRingSelection(-1),
      m_nCurrentlyViewedSkillSlot(0)
{
    InitAnalogStickRules();

    m_tSkillSelector.WrapUsingContainerControl(UISTACKPANEL_SKILLS_CONSOLE_ACTIVE_SKILLS);
    m_tSkillSelector.SetCallbacks(sSkillSelectionPredicate, sSkillSelectionChanged);

    m_tRuneSelector.WrapUsingContainerControl(UISTACKPANEL_SKILLS_CONSOLE_RUNES);
    m_tRuneSelector.SetCallbacks(sRuneSelectionPredicate, sRuneSelectionChanged);

    m_tTraitSelector.WrapUsingContainerControl(UIWRAPPANEL_SKILLS_CONSOLE_PASSIVE_SKILLS);
    m_tTraitSelector.SetCallbacks(sTraitSelectionPredicate, sTraitSelectionChanged);
}

inline BOOL PlayerSkillsGlobals::IsActive() const  // line 392; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return UI_GetWindowFromID(UIDLG_SKILLS_CONSOLE).IsActive();
}

inline void PlayerSkillsGlobals::OpenSelectedSlot()  // line 397; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    if (SlotIsSkill(GetSelectedSlot())) {
        SetSelectionMode(SKILL_SELECTION_MODE);
    } else {
        SetSelectionMode(TRAIT_SELECTION_MODE);
    }
}

inline void PlayerSkillsGlobals::CloseSelectedSlot()  // line 410; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    SetSelectionMode(SLOT_SELECTION_MODE);
}

inline void PlayerSkillsGlobals::GoBack()  // line 416; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    switch (m_eSelectionMode) {
        case SKILL_SELECTION_MODE:
        case TRAIT_SELECTION_MODE:
            CloseSelectedSlot();
            break;
        case RUNE_SELECTION_MODE:
            SetSelectionMode(SKILL_SELECTION_MODE);
            break;
        default:
            break;
    }
}

inline int32 PlayerSkillsGlobals::GetSelectedSlot() const  // line 436; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    const UIID & idSelectedIcon = tRingUI.GetSelectedControlID();
    if (idSelectedIcon != GetUIIDNone()) {
        return SlotFromIcon(idSelectedIcon);
    }
    return -1;
}

inline PlayerSkillsGlobals::SelectionMode PlayerSkillsGlobals::GetSelectionMode() const  // line 457; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return m_eSelectionMode;
}

inline void PlayerSkillsGlobals::InitAnalogStickRules()  // line 734; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);

    m_tStickInputFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL);
    m_tStickInputFilter.SetMinimumInputDelay(200);
}

PlayerSkillsGlobals * sgptSkillsGlobals;  // line 758

// 2.7.7: Kanai's powers panel, an unnamed 0x30-byte class shared with the leaderboard-inspect
// and equipment-manager skill screens. Constructor sub_710019b630 (this, parent UIID) fills the
// four owned 0xd8-byte children (constructor sub_710019aec0); the inline destructor frees them
// in reverse order with plain delete.
struct Unk19aec0 { uint8 unk_00[0xd8]; };
struct Unk19b630 {  // size 0x30
    ~Unk19b630()
    {
        delete unk_00[3];
        delete unk_00[2];
        delete unk_00[1];
        delete unk_00[0];
    }
    Unk19aec0 * unk_00[4];  // 0x00
    uint64 unk_20;  // 0x20
    UIID unk_28;  // 0x28
};
static_assert(sizeof(Unk19b630) == 0x30, "Unk19b630");
extern "C" void sub_710019b630(void * pThis, const UIID & idParent);
extern "C" BOOL sub_710019b960(Unk19b630 * pThis);  // any child scroll box can scroll
extern "C" void sub_710019b9d0(Unk19b630 * pThis);  // reset child scroll positions
extern "C" void sub_710019ba50(Unk19b630 * pThis, float flDelta);  // scroll child scroll boxes
extern "C" void sub_710019bab0(Unk19b630 * pThis, const Player * ptPlayer);  // refresh from player
static Unk19b630 * s_unk_186dc48;  // 0x710186dc48

const UIID & sGetActiveScrollBox(const PlayerSkillsGlobals::SelectionMode eSelectionMode)  // line 763
{
    switch (eSelectionMode) {
        case PlayerSkillsGlobals::SKILL_SELECTION_MODE:
            return UISCROLL_SKILLS_CONSOLE_SKILL_INFO;
        case PlayerSkillsGlobals::RUNE_SELECTION_MODE:
            return UISCROLL_SKILLS_CONSOLE_RUNE_INFO;
        case PlayerSkillsGlobals::TRAIT_SELECTION_MODE:
            return UISCROLL_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO;
        default:
            return GetUIIDNone();
    }
}

BOOL CanScrollDetails(const PlayerSkillsGlobals::SelectionMode eSelectionMode)  // line 785
{
    if (eSelectionMode == PlayerSkillsGlobals::SLOT_SELECTION_MODE) {
        return sub_710019b960(s_unk_186dc48);
    } else {
        const UIID & idScrollBox = sGetActiveScrollBox(eSelectionMode);
        if (idScrollBox == GetUIIDNone()) {
            return FALSE;
        }
        return UIUtils::Console::CanScrollContentInScrollBox(idScrollBox);
    }
}

void ResetDetailScrollPosition(const UIID & idScrollBox)  // line 809
{
    UI_GetConsoleScrollBoxFromID(idScrollBox).SetScrollPos(0.0f);
}

void ResetDetailScrollPositions()  // line 818
{
    ResetDetailScrollPosition(UISCROLL_SKILLS_CONSOLE_SKILL_INFO);
    ResetDetailScrollPosition(UISCROLL_SKILLS_CONSOLE_RUNE_INFO);
    ResetDetailScrollPosition(UISCROLL_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO);

    sub_710019b9d0(s_unk_186dc48);
}

void ScrollDetailInfo(const PlayerSkillsGlobals::SelectionMode eSelectionMode, const float nDelta)  // line 833
{
    if (eSelectionMode == PlayerSkillsGlobals::SLOT_SELECTION_MODE) {
        sub_710019ba50(s_unk_186dc48, nDelta);
    } else {
        const UIID & idScrollBox = sGetActiveScrollBox(eSelectionMode);
        if (idScrollBox != GetUIIDNone()) {
            UIUtils::Console::ScrollContentInScrollBox(idScrollBox, nDelta);
        }
    }
}

void PlayerSkillsGlobals::OnOpen(const Player * _ptPlayer)  // line 857
{
    ptPlayer = _ptPlayer;
    InitRingUI();
    SetSelectionMode(SLOT_SELECTION_MODE);
}

void PlayerSkillsGlobals::OnUpdate()  // line 868
{
    UpdateTouchInput();
    UpdateHeaderText();
    UpdateCategoryText();
    UpdateRuneVisibility();
    UpdateFocusGlows();
    UpdateButtonLegend();
    UpdateScrollBoxes();
}

static SkillCategory sSkillCategoryFromSlot(const int32 nSlot, const Player * ptPlayer)  // line 883
{
    SkillCategory eCategory = -1;
    if (SlotIsSkill(nSlot)) {
        const SNO snoPower = GetSkillForSlot(*ptPlayer, nSlot);
        if (snoPower != -1) {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL) {
                eCategory = GetSkillCategory(snoPower, *ptACDPlayer, FALSE);
            }
        } else {
            eCategory = UISkills::GetSkillCategoryForSlot(nSlot, ptPlayer->idPlayerACD);
        }
    }
    return eCategory;
}

#line 912
void PlayerSkillsGlobals::UpdateTouchInput()

{
    if (ptPlayer == NULL) {
        return;
    }
    if (m_eSelectionMode == SLOT_SELECTION_MODE) {
        return;
    }

#line 925
    static IVector2D vecLastAnchorPoint;

    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    if (tInputState.tControllerState[ptPlayer->nControllerIndex].tCurrentFrame.nNumTouches > 0) {


        const BOOL bIsFirstTouch = tInputState.tControllerState[ptPlayer->nControllerIndex].tPrevFrame.nNumTouches == 0;

        const TouchState & tTouch = tInputState.tControllerState[ptPlayer->nControllerIndex].tCurrentFrame.arTouches[0];
        if (bIsFirstTouch) {
            vecLastAnchorPoint.Set(tTouch.uX, tTouch.uY);
        } else {

#line 941
            BOOL bUpdateAnchorPoint = FALSE;
            if (abs(tTouch.uX - vecLastAnchorPoint.x) > s_nTouchDistanceThreshold) {
                if (tTouch.uX < vecLastAnchorPoint.x) {
                    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
                        MoveTraitSelector(DIRECTION_LEFT);
                    } else {
                        MoveSkillSelector(DIRECTION_LEFT);
                    }
                } else if (tTouch.uX > vecLastAnchorPoint.y) {
                    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
                        MoveTraitSelector(DIRECTION_RIGHT);
                    } else {
                        MoveSkillSelector(DIRECTION_RIGHT);
                    }
                }
                bUpdateAnchorPoint = TRUE;
            }
            if (m_eSelectionMode == TRAIT_SELECTION_MODE && abs(tTouch.uY - vecLastAnchorPoint.y) > s_nTouchDistanceThreshold) {
                if (tTouch.uY < vecLastAnchorPoint.y) {
                    MoveTraitSelector(DIRECTION_UP);
                } else if (tTouch.uY > vecLastAnchorPoint.y) {
                    MoveTraitSelector(DIRECTION_DOWN);
                }
                bUpdateAnchorPoint = TRUE;
            }
            if (bUpdateAnchorPoint) {

#line 989
                vecLastAnchorPoint.Set(tTouch.uX, tTouch.uY);
            }
        }
    }
}

void PlayerSkillsGlobals::UpdateHeaderText()  // line 998
{
    const int32 nSlot = m_nCurrentlyViewedSkillSlot;
    if (nSlot == KANAI_SLOT) {
        UI_GetTextFromID(UITXT_SKILLS_CONSOLE_HEADER).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
        return;
    }
    const SkillCategory eCategory = UISkills::GetSkillCategoryForSlot(nSlot, ptPlayer->idPlayerACD);
    ShowCategoryName(UITXT_SKILLS_CONSOLE_HEADER, nSlot, eCategory);
}

void PlayerSkillsGlobals::UpdateCategoryText()  // line 1016
{
    const int32 nSlot = GetSelectedSlot();
    if (nSlot == -1) {
        return;
    }
    if (nSlot == KANAI_SLOT) {
        UI_GetTextFromID(UITXT_SKILLS_CONSOLE_CATEGORY_NAME).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
        if (GetCraftLevel(3, *sgptSkillsGlobals->ptPlayer) == 0) {
            UI_GetTextFromID(UITXT_SKILLS_CONSOLE_CATEGORY_DESCRIPTION).SetText(StringListFetch("ConsoleSkills:Cat_Kanai_Locked_Description"), FALSE);
        } else {
            UI_GetTextFromID(UITXT_SKILLS_CONSOLE_CATEGORY_DESCRIPTION).SetText(StringListFetch("ConsoleSkills:Cat_Kanai_Description"), FALSE);
        }
        return;
    }
    {
        const SkillCategory eCategory = sSkillCategoryFromSlot(nSlot, ptPlayer);
        ShowCategoryName(UITXT_SKILLS_CONSOLE_CATEGORY_NAME, nSlot, eCategory);
        ShowCategoryDescription(UITXT_SKILLS_CONSOLE_CATEGORY_DESCRIPTION, ptPlayer, nSlot, eCategory);
    }
}

void PlayerSkillsGlobals::UpdateFocusGlows()  // line 1050
{
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_MENU_GLOW).SetActive(m_eSelectionMode != SLOT_SELECTION_MODE);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RING_GLOW).SetActive(m_eSelectionMode == SLOT_SELECTION_MODE);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_ACTIVE_SKILLS_GLOW).SetActive(m_eSelectionMode == SKILL_SELECTION_MODE);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RUNES_GLOW).SetActive(m_eSelectionMode == RUNE_SELECTION_MODE);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS_GLOW).SetActive(m_eSelectionMode == TRAIT_SELECTION_MODE);
}

void PlayerSkillsGlobals::UpdateRuneVisibility()  // line 1063
{
    BOOL bShowRunes = FALSE;
    if (m_eSelectionMode == SLOT_SELECTION_MODE) {
        const SNO snoSkill = GetSkillForSlot(*ptPlayer, GetSelectedSlot());
        bShowRunes = snoSkill != -1;
    } else if (m_eSelectionMode != TRAIT_SELECTION_MODE) {
        bShowRunes = PlayerMeetsSkillLevelRequirement(ptPlayer->idPlayerACD, GetSelectedSkill());
    }
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RUNES).SetActive(bShowRunes);
}

void PlayerSkillsGlobals::Refresh()  // line 1084
{
    if (IsActive()) {
        UpdateSkillRing();
        RefreshUI();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void PlayerSkillsGlobals::RefreshUI()  // line 1101
{
    const int32 nSlot = GetSelectedSlot();
    if (nSlot == -1) {
        return;
    }

    const BOOL fIsSkill = SlotIsSkill(nSlot);
    const BOOL fIsPassive = SlotIsPassive(nSlot);

    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_ACTIVE_SKILLS).SetActive(fIsSkill);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS).SetActive(fIsPassive);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS).SetActive(!(fIsSkill || fIsPassive));

    if (fIsSkill) {
        UpdateSkillAndRuneUIs();
    } else if (fIsPassive) {
        UpdateTraitsListUI();
        UpdateTraitInfoUI();
    } else {
        UpdateKanaisPowersUI();
    }
}

void PlayerSkillsGlobals::UpdateSkillAndRuneUIs()  // line 1136
{
    UpdateSkillsListUI();
    UpdateSkillInfoUI();
    UpdateRunesListUI();
    UpdateRuneInfoUI();
}

void PlayerSkillsGlobals::UpdateKanaisPowersUI()  // line 1178
{
    sub_710019bab0(s_unk_186dc48, ptPlayer);
}

void PlayerSkillsGlobals::UpdateSkillsListUI()  // line 1198
{
    for (int32 i = 0; i < MAX_SKILLS_PER_CATEGORY_CONSOLE; i++) {
        UpdateSkillItemUI(i, FALSE);
    }

    const BOOL bShowArrows = m_eSelectionMode == SKILL_SELECTION_MODE &&
                             (Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge());
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_ELECTIVE_MODE).SetActive(bShowArrows);
}

#line 1213
UIID GetUIIDForSkillItem(
    const int32 nIndex,
    const BOOL bIsTrait)
{
    const UIID & idParent = bIsTrait ? UIWRAPPANEL_SKILLS_CONSOLE_PASSIVE_SKILLS : UISTACKPANEL_SKILLS_CONSOLE_ACTIVE_SKILLS;
    CRefString sItemID;
    sItemID.Format(".%s%d", bIsTrait ? "Passive" : "Skill", nIndex + 1);
    return UIIDCreateFromParent(idParent, sItemID.str());
}

AssignmentType IsSkillAssigned(const Player & tPlayer, const int32 nSlot, const SNO snoPower, const BOOL bIsTrait)  // line 1226
{
    if (bIsTrait) {
        AssignmentType eAssignment = NOT_ASSIGNED;
        const int32 nTraitSlot = GetTraitSlotNumber(nSlot);
        for (int32 i = 0; i < 4; i++) {
            if (GetTraitForSlot(tPlayer, i) == snoPower) {
                eAssignment = (i == nTraitSlot) ? ASSIGNED_IN_SLOT : ASSIGNED_IN_ANOTHER_SLOT;
                break;
            }
        }
        return eAssignment;
    } else if (Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge()) {
        for (int32 i = 0; i < 6; i++) {
            if (GetSkillForSlot(tPlayer, i) == snoPower) {
                return (i == nSlot) ? ASSIGNED_IN_SLOT : ASSIGNED_IN_ANOTHER_SLOT;
            }
        }
    } else {
        const SNO snoEquippedPower = GetSkillForSlot(tPlayer, nSlot);
        return (snoEquippedPower == snoPower) ? ASSIGNED_IN_SLOT : NOT_ASSIGNED;
    }
    return NOT_ASSIGNED;
}

BOOL ShouldShowSkillAsNew(const Player & tPlayer, const int32 nSlot, const SkillListEntry & tSkillEntry, const BOOL bIsTrait)  // line 1275
{
    if (bIsTrait) {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer != NULL) {
            const int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
            const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(tPlayer.nPlayerIndex);
            return IsTraitNew(tSkillEntry, nLocalPlayerIndex, nLevel);
        }
        return FALSE;
    }
    return HasNewSkillOrRune(nSlot, tSkillEntry.snoPower);
}

#line 1302
void PlayerSkillsGlobals::UpdateSkillItemUI(
    const int32 nIndex,
    const BOOL bIsTrait)
{
    UIID idItem = GetUIIDForSkillItem(nIndex, bIsTrait);

    const int32 nNumSkills = bIsTrait ? GetNumTraits() : GetNumSkills(m_nCurrentlyViewedSkillSlot);
    if (nIndex >= nNumSkills) {
        UI_GetWindowFromID(idItem).SetActive(FALSE);
        return;
    }
    UI_GetWindowFromID(idItem).SetActive(TRUE);


    const SkillListEntry & tSkillEntry = bIsTrait ? GetTrait(nIndex) : GetSkill(m_nCurrentlyViewedSkillSlot, nIndex);


    const BOOL bLocked = !PlayerMeetsSkillLevelRequirement(ptPlayer->idPlayerACD, &tSkillEntry);
    UpdateLockedIndicator(idItem, bLocked);


    const AssignmentType eAssignment = IsSkillAssigned(*ptPlayer, GetSelectedSlot(), tSkillEntry.snoPower, bIsTrait);
    UpdateAssignedIndicator(idItem, eAssignment);


    UIID idIcon;
    idIcon.SetFromParent(idItem, "LayoutRoot.Icon");

    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(tSkillEntry.snoPower, -1);

    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
    tIcon.SetPlayer(ptPlayer);
    tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

#line 1341
    BOOL bGrayedOut = bLocked;
    if (GameIsWeeklyChallenge() && !ChallengeRift::IsValidChallengeRiftSkill(tSkillEntry.snoPower)) {
        bGrayedOut = TRUE;
    }

    if (!bGrayedOut) {


        const int32 nSlot = GetSelectedSlot();
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        const BOOL bSlotIsLocked = nSlot != -1 && ptACDPlayer != NULL && !IsSlotUnlocked(*ptACDPlayer, nSlot);
        const BOOL bHasFocus = bIsTrait ? m_eSelectionMode == TRAIT_SELECTION_MODE : m_eSelectionMode == SKILL_SELECTION_MODE;
        if (bSlotIsLocked || (eAssignment != ASSIGNED_IN_SLOT && !bHasFocus)) {
            bGrayedOut = TRUE;
        }
    }

    tIcon.SetShader(bGrayedOut ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE) : -1);


    const SkillListEntry * ptSelectedSkill = bIsTrait ? GetSelectedTrait() : GetSelectedSkill();
    const SelectionMode eRequiredSelectionMode = bIsTrait ? TRAIT_SELECTION_MODE : SKILL_SELECTION_MODE;
    const BOOL bSelected = m_eSelectionMode == eRequiredSelectionMode && ptSelectedSkill != NULL && ptSelectedSkill->snoPower == tSkillEntry.snoPower;
    UpdateSelectionHighlight(idItem, bSelected);


    const BOOL bIsNew = ShouldShowSkillAsNew(*ptPlayer, m_nCurrentlyViewedSkillSlot, tSkillEntry, bIsTrait);
    UpdateNewItemIndicator(idItem, bIsNew);
}

void PlayerSkillsGlobals::UpdateSkillInfoUI()  // line 1372
{
    SNO snoPower = -1;
    if (m_eSelectionMode == SLOT_SELECTION_MODE) {
        const int32 nSlot = GetSelectedSlot();
        snoPower = GetSkillForSlot(*ptPlayer, nSlot);
    } else {
        const SkillListEntry * ptSelectedSkill = GetSelectedSkill();
        if (ptSelectedSkill != NULL) {
            snoPower = ptSelectedSkill->snoPower;
        }
    }

    CRefString sSkillName = (snoPower != -1) ? GetSkillName(snoPower) : StringListFetch("ConsoleSkills:Skills");
    if (m_eSelectionMode == SKILL_SELECTION_MODE) {
        Set_Text_Color(sSkillName, 0);
    }
    UI_GetTextFromID(UITXT_SKILLS_CONSOLE_SKILL_NAME).SetText(sSkillName.str(), FALSE);

    if (snoPower == -1) {
        UI_GetTextFromID(UITXT_SKILLS_CONSOLE_SKILL_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    } else {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL) {
            ShowSkillDescription(UITXT_SKILLS_CONSOLE_SKILL_INFO, snoPower, *ptACDPlayer, FALSE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
        }
    }
}

void PlayerSkillsGlobals::UpdateRunesListUI()  // line 1415
{
    for (int32 i = 0; i < MAX_RUNE_RANKS_CONSOLE; i++) {
        UpdateRunesListItemUI(i);
    }
}

void PlayerSkillsGlobals::UpdateRunesListItemUI(const int32 nIndex)  // line 1427
{
    UIID idRuneItem = UIIDCreateFromParent(UISTACKPANEL_SKILLS_CONSOLE_RUNES, CRefString().Format(".Rune%d", nIndex + 1));
    if (nIndex >= GetNumRunes()) {
        UI_GetWindowFromID(idRuneItem).SetActive(FALSE);
        return;
    }
    UI_GetWindowFromID(idRuneItem).SetActive(TRUE);

    const RuneEntry * ptRuneEntry = GetRune(nIndex);
    const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }

    const SNO snoPower = (GetSelectedSkill() != NULL) ? GetSelectedSkill()->snoPower : GetSkillForSlot(*ptPlayer, GetSelectedSlot());
    const RuneType eRune = (snoPower != -1) ? PowerGetRuneTypeForACD(*ptACDPlayer, snoPower) : -1;
    const BOOL bEquipped = eRune == ptRuneEntry->eRuneType;
    const BOOL bLocked = !PlayerMeetsRuneLevelRequirement(ptPlayer->idPlayerACD, ptRuneEntry);
    UpdateLockedIndicator(idRuneItem, bLocked);

    UIID idIcon;
    idIcon.SetFromParent(idRuneItem, "LayoutRoot.Icon");
    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
    CRefString sRuneTexture = GetRuneTextureFrame(ptRuneEntry->eRuneType, bEquipped);
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo(sRuneTexture.str(), tTexFrame);
    tIcon.SetAllTextureFrames(tTexFrame);

    BOOL bGrayedOut = bLocked;
    if (GameIsWeeklyChallenge() && ChallengeRift::GetRuneTypeForSkill(snoPower) != ptRuneEntry->eRuneType) {
        bGrayedOut = TRUE;
    }
    if (!bEquipped && !bGrayedOut) {
        bGrayedOut = m_eSelectionMode != RUNE_SELECTION_MODE;
    }
    tIcon.SetShader(bGrayedOut ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE) : -1);

    UpdateAssignedIndicator(idRuneItem, bEquipped ? ASSIGNED_IN_SLOT : NOT_ASSIGNED);

    const RuneEntry * ptSelectedRune = GetSelectedRune();
    const BOOL bSelected = ptSelectedRune != NULL && ptSelectedRune->eRuneType == ptRuneEntry->eRuneType && m_eSelectionMode == RUNE_SELECTION_MODE;
    UpdateSelectionHighlight(idRuneItem, bSelected);

    BOOL bIsNew = FALSE;
    const SkillListEntry * ptSkill;
    if (m_eSelectionMode == SLOT_SELECTION_MODE) {
        const SNO snoPowerScoped = GetSkillForSlot(*ptPlayer, GetSelectedSlot());
        ptSkill = GetSkillWithSNO(m_nCurrentlyViewedSkillSlot, snoPowerScoped);
    } else {
        ptSkill = GetSelectedSkill();
    }
    if (ptSkill != NULL) {
        const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);
        const int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        bIsNew = EntryHasNewRune(*ptSkill, *ptRuneEntry, nLocalPlayerIndex, nLevel);
    }
    UpdateNewItemIndicator(idRuneItem, bIsNew);
}

void PlayerSkillsGlobals::UpdateRuneInfoUI()  // line 1517
{
    RuneType eRune = -1;
    SNO snoPower = -1;
    if (m_eSelectionMode == SLOT_SELECTION_MODE) {
        snoPower = GetSkillForSlot(*ptPlayer, GetSelectedSlot());
    } else {
        const SkillListEntry * ptSelectedSkill = GetSelectedSkill();
        if (ptSelectedSkill != NULL) {
            snoPower = ptSelectedSkill->snoPower;
        }
    }

    if (m_eSelectionMode != RUNE_SELECTION_MODE) {
        const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL) {
            eRune = PowerGetRuneTypeForACD(*ptACDPlayer, snoPower);
        }
    } else {
        const RuneEntry * ptSelectedRune = GetSelectedRune();
        if (ptSelectedRune != NULL) {
            eRune = ptSelectedRune->eRuneType;
        }
    }

    CRefString sRuneName = (eRune != -1) ? PowerGetRuneName(snoPower, eRune) : StringListFetch("ConsoleSkills:SkillRunes");
    if (m_eSelectionMode == RUNE_SELECTION_MODE) {
        Set_Text_Color(sRuneName, 0);
    }
    UI_GetTextFromID(UITXT_SKILLS_CONSOLE_RUNE_NAME).SetText(sRuneName.str(), FALSE);

    if (eRune == -1) {
        UI_GetTextFromID(UITXT_SKILLS_CONSOLE_RUNE_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    } else {
        ShowRuneDescription(UITXT_SKILLS_CONSOLE_RUNE_INFO, snoPower, eRune, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
        ResetDetailScrollPositions();
    }
}

void PlayerSkillsGlobals::UpdateTraitsListUI()  // line 1572
{
    for (int32 i = 0; i < 20; i++) {
        UpdateSkillItemUI(i, TRUE);
    }
}

void PlayerSkillsGlobals::UpdateTraitInfoUI()  // line 1584
{
    SNO snoPower = -1;
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        const SkillListEntry * ptSelectedTrait = GetSelectedTrait();
        if (ptSelectedTrait != NULL) {
            snoPower = ptSelectedTrait->snoPower;
        }
    } else {
        snoPower = GetTraitForSlot(*ptPlayer, GetTraitSlotNumber(GetSelectedSlot()));
    }

    CRefString sPowerName;
    if (!UISkills::GetName(snoPower, sPowerName)) {
        sPowerName = StringListFetch("ConsoleSkills:PassiveSkillsTitle");
    }
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        Set_Text_Color(sPowerName, 0);
    }
    UI_GetTextFromID(UITXT_SKILLS_CONSOLE_PASSIVE_SKILLS_NAME).SetText(sPowerName.str(), FALSE);

    if (snoPower == -1) {
        UI_GetTextFromID(UITXT_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned").str(), FALSE);
    } else {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL) {
            ShowSkillDescription(UITXT_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO, snoPower, *ptACDPlayer, TRUE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
        }
    }
}

BOOL sShouldIncludeInRing(const UIID & idControl)  // line 1628
{
    return idControl != UIWND_SKILLS_CONSOLE_RING_GLOW && idControl != UIWND_SKILLS_CONSOLE_PASSIVE_RING_GLOW;
}

void PlayerSkillsGlobals::InitRingUI()  // line 1637
{
    tRingUI.WrapUsingContainerControl(UIWND_SKILLS_CONSOLE_RING, UIWND_SKILLS_CONSOLE_RING_CURSOR, sShouldIncludeInRing, FALSE);
}

const SkillListEntry * PlayerSkillsGlobals::GetSelectedSkill() const  // line 1646
{
    if (m_nSelectedSkillIndex >= 0 && m_nSelectedSkillIndex < GetNumSkills(m_nCurrentlyViewedSkillSlot)) {
        return &GetSkill(m_nCurrentlyViewedSkillSlot, m_nSelectedSkillIndex);
    }
    return NULL;
}

const RuneEntry * PlayerSkillsGlobals::GetSelectedRune() const  // line 1661
{
    if (m_nSelectedRuneIndex >= 0 && m_nSelectedRuneIndex < GetNumRunes()) {
        return GetRune(m_nSelectedRuneIndex);
    }
    return NULL;
}

const SkillListEntry * PlayerSkillsGlobals::GetSelectedTrait() const  // line 1675
{
    if (m_nSelectedTraitIndex >= 0 && m_nSelectedTraitIndex < GetNumTraits()) {
        return &GetTrait(m_nSelectedTraitIndex);
    }
    return NULL;
}

void PlayerSkillsGlobals::RefreshSkillsList()  // line 1689
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }
    ScopedSkillKitDefinitionLockR pdefSkillKit(ptACDPlayer->GetSkillKit());
    for (int32 i = 0; i < 42; i++) {
        m_tSkills[i].clear();
        GetSkillListForSlot(pdefSkillKit, ptPlayer->idPlayerACD, m_tSkills[i], i);
    }
}

int32 PlayerSkillsGlobals::GetNumSkills(const int32 nSlot) const  // line 1711
{
    return m_tSkills[nSlot].size();
}

#line 1720
const SkillListEntry & PlayerSkillsGlobals::GetSkill(
    const int32 nSlot,
    const int32 nIndex) const
{
    return m_tSkills[nSlot][nIndex];
}

const SkillListEntry * PlayerSkillsGlobals::GetSkillWithSNO(const int32 nSlot, const SNO snoPower) const  // line 1730
{
    const int32 nIndex = GetIndexOfSkill(nSlot, snoPower);
    return (nIndex != -1) ? &GetSkill(nSlot, nIndex) : NULL;
}

int32 PlayerSkillsGlobals::GetIndexOfSkill(const int32 nSlot, const SNO snoPower) const  // line 1741
{
    for (int32 i = 0; i < m_tSkills[nSlot].size(); i++) {
        if (m_tSkills[nSlot][i].snoPower == snoPower) {
            return i;
        }
    }
    return -1;
}

const SkillListEntry * PlayerSkillsGlobals::GetSkillForVisibleRunes() const  // line 1759
{
    return (m_eSelectionMode != SLOT_SELECTION_MODE)
               ? GetSelectedSkill()
               : GetSkillWithSNO(m_nCurrentlyViewedSkillSlot, GetSkillForSlot(*ptPlayer, GetSelectedSlot()));
}

int32 PlayerSkillsGlobals::GetNumRunes() const  // line 1768
{
    const SkillListEntry * ptSkill = GetSkillForVisibleRunes();
    return (ptSkill != NULL) ? MAX_RUNE_RANKS_CONSOLE : 0;
}

const RuneEntry * PlayerSkillsGlobals::GetRune(const int32 nIndex) const  // line 1778
{
    const SkillListEntry * ptSkill = GetSkillForVisibleRunes();
    if (ptSkill == NULL) {
        return NULL;
    }
    return &ptSkill->tRunes[nIndex];
}

#line 1793
int32 PlayerSkillsGlobals::GetIndexOfRune(
    const RuneType eRuneType) const
{
    const SkillListEntry * ptSkill = GetSkillForVisibleRunes();
    if (ptSkill != NULL)
    {
        for (int32 i = 0; i < MAX_RUNE_RANKS_CONSOLE; i++)
        {
            if (ptSkill->tRunes[i].eRuneType == eRuneType)
            {
                return i;
            }
        }
    }
    return -1;
}

int32 PlayerSkillsGlobals::GetNumTraits() const  // line 1814
{
    return m_tTraits.size();
}

void PlayerSkillsGlobals::RefreshTraitsList()  // line 1823
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }
    ScopedSkillKitDefinitionLockR pdefSkillKit(ptACDPlayer->GetSkillKit());
    GetTraitList(pdefSkillKit, PlayerGetPlayerClass(ptPlayer), m_tTraits);
}

const SkillListEntry & PlayerSkillsGlobals::GetTrait(const int32 nIndex) const  // line 1841
{
    return m_tTraits[nIndex];
}

int32 PlayerSkillsGlobals::GetIndexOfTrait(const SNO snoPower) const  // line 1850
{
    for (int32 i = 0; i < m_tTraits.size(); i++) {
        if (m_tTraits[i].snoPower == snoPower) {
            return i;
        }
    }
    return -1;
}

void PlayerSkillsGlobals::SetSelectionMode(const SelectionMode eMode)  // line 1867
{
    if (m_eSelectionMode == eMode) {
        return;
    }
    m_eSelectionMode = eMode;

    switch (eMode) {
        case SLOT_SELECTION_MODE:
            EnableRingUI(TRUE);
            UI_SetControllerFocus(UIWND_SKILLS_CONSOLE_RING);
            m_tSkillSelector.SetSelectedIndex(-1, FALSE);
            m_tRuneSelector.SetSelectedIndex(-1, FALSE);
            m_tTraitSelector.SetSelectedIndex(-1, FALSE);
            break;
        case SKILL_SELECTION_MODE: {
            EnableRingUI(FALSE);
            UI_SetControllerFocus(UIWND_SKILLS_CONSOLE_ACTIVE_SKILLS);
            const SNO snoPower = GetSkillForSlot(*ptPlayer, GetSelectedSlot());
            const int32 nIndex = GetIndexOfSkill(m_nCurrentlyViewedSkillSlot, snoPower);
            m_tSkillSelector.SetSelectedIndex(Math::Max(nIndex, 0), FALSE);
            m_tRuneSelector.SetSelectedIndex(-1, FALSE);
            break;
        }
        case RUNE_SELECTION_MODE:
            if (GameIsWeeklyChallenge()) {
                const int32 nSelectedRuneIndex = GetIndexOfRune(ChallengeRift::GetRuneTypeForSkill(GetSelectedSkill()->snoPower));
                m_tRuneSelector.SetSelectedIndex(nSelectedRuneIndex, FALSE);
            } else {
                m_tRuneSelector.SetSelectedIndex(0, FALSE);
            }
            break;
        case TRAIT_SELECTION_MODE: {
            EnableRingUI(FALSE);
            UI_SetControllerFocus(UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS);
            const int32 nSlot = GetTraitSlotNumber(GetSelectedSlot());
            const SNO snoPower = GetTraitForSlot(*ptPlayer, nSlot);
            const int32 nIndex = GetIndexOfTrait(snoPower);
            m_tTraitSelector.SetSelectedIndex(Math::Max(nIndex, 0), FALSE);
            break;
        }
    }

    RefreshUI();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerSkillsGlobals::UpdateButtonLegend()  // line 1941
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!tActionManager.AreActionsDirty()) {
        return;
    }
    tActionManager.SetActionsDirtyFlag(FALSE);
    tActionManager.ClearAllActions();

    const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }

    switch (m_eSelectionMode) {
        case SLOT_SELECTION_MODE: {
            const int32 nSlot = GetSelectedSlot();
            if (nSlot == KANAI_SLOT) {
                if (CanScrollDetails(m_eSelectionMode)) {
                    CRefString sScrollPrompt = StringListFetch("ConsoleSkills:ScrollPowerDescriptions");
                    tActionManager.SetAction(10, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
                    tActionManager.SetAction(11, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
                }
            } else if (PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE)) {
                tActionManager.SetAction(4, ACTION_OPEN_SLOT, StringListFetch("ConsoleSkills:SelectSkillSlot").str());
            }
            tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
            if (PlayerCanOpenInventory(*ptACDPlayer)) {
                tActionManager.SetAction(5, ACTION_SHOW_CHARACTER_DETAILS, StringListFetch("ConsoleSkills:CharacterDetails").str());
            }
            if (SlotIsSkill(nSlot) && !IsWaitingForCooldown(nSlot) && !IsInTieredRift()) {
                tActionManager.SetAction(6, ACTION_CLEAR_SKILL, StringListFetch("ConsoleSkills:Clear").str());
            }
            break;
        }
        case SKILL_SELECTION_MODE:
            if (IsSlotUnlocked(*ptACDPlayer, GetSelectedSlot()) && (IsInTieredRift() || CanAssignSkill(GetSelectedSkill()))) {
                tActionManager.SetAction(4, ACTION_ASSIGN_SKILL, StringListFetch("ConsoleSkills:AssignSkill").str());
            }
            tActionManager.SetAction(7, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
            break;
        case RUNE_SELECTION_MODE:
            if (IsSlotUnlocked(*ptACDPlayer, GetSelectedSlot()) && (IsInTieredRift() || CanAssignRune(GetSelectedRune()))) {
                tActionManager.SetAction(4, ACTION_ASSIGN_RUNE, StringListFetch("ConsoleSkills:AssignRune").str());
            }
            tActionManager.SetAction(7, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
            break;
        case TRAIT_SELECTION_MODE:
            if (IsSlotUnlocked(*ptACDPlayer, GetSelectedSlot()) &&
                (IsInTieredRift() || CanAssignTrait(GetSelectedTrait()))) {
                tActionManager.SetAction(4, ACTION_ASSIGN_PASSIVE, StringListFetch("ConsoleSkills:AssignSkill").str());
            }
            tActionManager.SetAction(7, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
            break;
    }

    if (m_eSelectionMode != SLOT_SELECTION_MODE && CanScrollDetails(m_eSelectionMode)) {
        CRefString sScrollPrompt = StringListFetch("ConsoleSkills:ScrollSkillDescriptions");
        tActionManager.SetAction(10, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
        tActionManager.SetAction(11, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
    }

    ::Console::Screens::UpdateButtonLegendText();
}

void sUpdateScrollBox(const UIID & idScrollBox, const UIID & idDimmer)  // line 2033
{
    const UIConsoleScrollBox & tScrollBox = UI_GetConsoleScrollBoxFromID(idScrollBox);
    const BOOL bCanScroll = tScrollBox.GetScrollLimit() > EPSILON &&
                            Math::Abs(tScrollBox.GetScrollLimit() - tScrollBox.GetScrollPos()) > EPSILON;
    UI_GetWindowFromID(idDimmer).SetActive(bCanScroll);
}

void PlayerSkillsGlobals::UpdateScrollBoxes()  // line 2045
{
    sUpdateScrollBox(UISCROLL_SKILLS_CONSOLE_SKILL_INFO, UIWND_SKILLS_CONSOLE_SKILL_INFO_DIMMER);
    sUpdateScrollBox(UISCROLL_SKILLS_CONSOLE_RUNE_INFO, UIWND_SKILLS_CONSOLE_RUNE_INFO_DIMMER);
    sUpdateScrollBox(UISCROLL_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO, UIWND_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO_DIMMER);
}



#line 2056
void PlayerSkillsGlobals::AssignSelectedSkill()

{
    const SkillListEntry * ptSelectedSkill = GetSelectedSkill();
    if (ptSelectedSkill == NULL) {
        return;
    }


    const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }

#line 2073
    const int32 nSlot = GetSelectedSlot();
    const SNO snoCurrentPower = GetSkillForSlot(*ptPlayer, nSlot);
    const RuneType eCurrentRune = PowerGetRuneTypeForACD(*ptACDPlayer, snoCurrentPower);
    const BOOL bPreserveRune = snoCurrentPower == ptSelectedSkill->snoPower && eCurrentRune != -1;

    SendAssignSkillMessage(bPreserveRune);


    AnimateSelection();
    AnimateLoadoutEquip();


    const int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);

    for (int32 i = 0; i < MAX_RUNE_RANKS_CONSOLE; i++) {
        if (ptSelectedSkill->tRunes[i].nLevel <= nPlayerLevel) {


            SetSelectionMode(RUNE_SELECTION_MODE);
            if (bPreserveRune) {


                int32 nIndex = GetIndexOfRune(eCurrentRune);
                m_tRuneSelector.SetSelectedIndex((nIndex != -1) ? nIndex : 0, FALSE);
            }
            break;
        }
    }


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SKILL), cgtPSODefaults);
}

void PlayerSkillsGlobals::AssignSelectedRune()  // line 2109
{
    SendAssignSkillMessage(TRUE);
    AnimateSelection();
    AnimateLoadoutEquip();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_RUNE), cgtPSODefaults);
}

void PlayerSkillsGlobals::AssignSelectedTrait()  // line 2125
{
    if (SendAssignTraitMessage()) {
        AnimateSelection();
        AnimateLoadoutEquip();
    }
    CloseSelectedSlot();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SKILL), cgtPSODefaults);
}

void PlayerSkillsGlobals::EnableRingUI(const BOOL bEnable)  // line 2144
{
    tRingUI.SetEnabled(bEnable);
    UpdateSkillRing();
}

::Console::RadioSelectorWrapper * PlayerSkillsGlobals::GetFocusedRadioSelector()  // line 2154
{
    switch (m_eSelectionMode) {
        case SKILL_SELECTION_MODE:
            return &m_tSkillSelector;
        case RUNE_SELECTION_MODE:
            return &m_tRuneSelector;
        case TRAIT_SELECTION_MODE:
            return &m_tTraitSelector;
        default:
            return NULL;
    }
}

void PlayerSkillsGlobals::MoveTraitSelector(const SelectorDirection eDirection)  // line 2178
{
    const int32 nSelectedIndex = m_tTraitSelector.GetSelectedIndex();
    const UIID & idGrid = m_tTraitSelector.GetContainerControlID();
    UIWrapPanel & tTraitGrid = UI_GetWrapPanelFromID(idGrid);
    const int32 nNumItemsInRow = tTraitGrid.GetNumChildrenInStrip(tTraitGrid.GetStripContainingChild(nSelectedIndex));
    const int32 nNumRows = tTraitGrid.GetNumStrips();
    IVector2D vecSelectedGridPos;
    const BOOL bIsValidPos = tTraitGrid.GetGridPosOfChild(nSelectedIndex, vecSelectedGridPos);
    int32 nNewSelectedIndex;
    if (bIsValidPos) {
        switch (eDirection) {
            case DIRECTION_LEFT:
                if (vecSelectedGridPos.x > 0) {
                    vecSelectedGridPos.x--;
                }
                break;
            case DIRECTION_RIGHT:
                if (vecSelectedGridPos.x < nNumItemsInRow - 1) {
                    vecSelectedGridPos.x++;
                }
                break;
            case DIRECTION_UP:
                if (vecSelectedGridPos.y > 0) {
                    vecSelectedGridPos.y--;
                }
                break;
            case DIRECTION_DOWN:
                if (vecSelectedGridPos.y < nNumRows - 1) {
                    vecSelectedGridPos.y++;
                }
                break;
        }
        nNewSelectedIndex = tTraitGrid.GetChildIndexAtGridPos(vecSelectedGridPos);
        if (nNewSelectedIndex == -1) {
            return;
        }
    } else {
        nNewSelectedIndex = Math::Max(nSelectedIndex, 0);
    }
    if (nNewSelectedIndex != m_tTraitSelector.GetSelectedIndex()) {
        m_tTraitSelector.SetSelectedIndex(nNewSelectedIndex, TRUE);
    }
}

void PlayerSkillsGlobals::MoveSkillSelector(const SelectorDirection eDirection)  // line 2238
{
    ::Console::RadioSelectorWrapper * ptCurrentSelector = GetFocusedRadioSelector();
    if (ptCurrentSelector == NULL) {
        return;
    }
    if (eDirection == DIRECTION_RIGHT) {
        if (ptCurrentSelector->SelectNext(TRUE)) {
            return;
        }
        if (m_eSelectionMode != SKILL_SELECTION_MODE ||
            !(Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge())) {
            return;
        }
        m_nCurrentlyViewedSkillSlot = (m_nCurrentlyViewedSkillSlot + 1) % 6;
        ptCurrentSelector->SetSelectedIndex(0, TRUE);
    } else if (eDirection == DIRECTION_LEFT) {
        if (ptCurrentSelector->SelectPrevious(TRUE)) {
            return;
        }
        if (m_eSelectionMode != SKILL_SELECTION_MODE ||
            !(Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge())) {
            return;
        }
        m_nCurrentlyViewedSkillSlot = (m_nCurrentlyViewedSkillSlot - 1 < 0) ? 5 : m_nCurrentlyViewedSkillSlot - 1;
        ptCurrentSelector->SetSelectedIndex(GetNumSkills(m_nCurrentlyViewedSkillSlot) - 1, TRUE);
    } else {
        return;
    }
    UpdateSkillAndRuneUIs();
}

void PlayerSkillsGlobals::SendAssignSkillMessage(const BOOL bAssignRune)  // line 2280
{
    const SkillListEntry * ptSelectedSkill = GetSelectedSkill();
    if (ptSelectedSkill == NULL) {
        return;
    }
    RuneType eRuneType = -1;
    if (bAssignRune) {
        const RuneEntry * ptSelectedRune = GetSelectedRune();
        if (ptSelectedRune == NULL) {
            return;
        }
        eRuneType = ptSelectedRune->eRuneType;
    }
    AssignSkillMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.snoPower = ptSelectedSkill->snoPower;
    tMessage.nRuneType = eRuneType;
    tMessage.nSlot = GetSelectedSlot();
    MessageSendToServer(CMSG_TRY_ASSIGN_SKILL_TO_SLOT, &tMessage, sizeof(tMessage), sgptSkillsGlobals->ptPlayer);
}

BOOL PlayerSkillsGlobals::SendAssignTraitMessage()  // line 2314
{
    BOOL bChanged = FALSE;
    const int32 nSelectedSlot = GetTraitSlotNumber(GetSelectedSlot());
    const SNO snoSelectedPower = GetSelectedTrait()->snoPower;
    AssignTraitsMessage tMessage;
    for (int32 i = 0; i < 4; i++) {
        const SNO snoPower = GetTraitForSlot(*ptPlayer, i);
        tMessage.snoPower[i] = snoPower;
        if (i == nSelectedSlot) {
            if (snoSelectedPower != snoPower) {
                tMessage.snoPower[i] = snoSelectedPower;
                bChanged = TRUE;
            }
        } else if (snoPower == snoSelectedPower) {
            return FALSE;
        }
    }
    if (!bChanged) {
        return FALSE;
    }
    MessageSendToServer(CMSG_TRY_ASSIGN_TRAITS, &tMessage, sizeof(tMessage), ptPlayer);
    return TRUE;
}

void PlayerSkillsGlobals::AnimateSelection()  // line 2358
{
    ::Console::RadioSelectorWrapper * ptSelector;
    switch (m_eSelectionMode) {
        case SKILL_SELECTION_MODE:
            ptSelector = &m_tSkillSelector;
            break;
        case RUNE_SELECTION_MODE:
            ptSelector = &m_tRuneSelector;
            break;
        case TRAIT_SELECTION_MODE:
            ptSelector = &m_tTraitSelector;
            break;
        default:
            return;
    }
    const int32 nSelectedIndex = ptSelector->GetSelectedIndex();
    if (nSelectedIndex != -1) {
        const UIID & ptidItem = ptSelector->GetUIIDAtIndex(nSelectedIndex);
        AnimateSkillSelection(ptidItem);
    }
}

void PlayerSkillsGlobals::AnimateHighlight()  // line 2399
{
    ::Console::RadioSelectorWrapper * ptSelector;
    switch (m_eSelectionMode) {
        case SKILL_SELECTION_MODE:
            ptSelector = &m_tSkillSelector;
            break;
        case RUNE_SELECTION_MODE:
            ptSelector = &m_tRuneSelector;
            break;
        case TRAIT_SELECTION_MODE:
            ptSelector = &m_tTraitSelector;
            break;
        default:
            return;
    }
    const int32 nSelectedIndex = ptSelector->GetSelectedIndex();
    if (nSelectedIndex != -1) {
        const UIID & ptidItem = ptSelector->GetUIIDAtIndex(nSelectedIndex);
        AnimateSkillHighlight(ptidItem);
    }
}

void PlayerSkillsGlobals::AnimateLoadoutEquip()  // line 2440
{
    const UIID & idSlot = tRingUI.GetSelectedControlID();
    AnimateEquippedSkillIcon(idSlot);
}

void PlayerSkillsGlobals::OnLeftStick(const UIID & id, const int32 nControllerIndex)  // line 2510
{
    const Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    switch (m_eSelectionMode) {
        case SLOT_SELECTION_MODE:
            if (tRingStickInputFilter.HandleInput(vecStick, FALSE) == ::Console::AnalogStickInputFilter::Rule::CATCH_ALL_INPUT) {
                tRingUI.UpdateInput(nControllerIndex, IconSelectionChanged, NULL);
            }
            break;
        case TRAIT_SELECTION_MODE: {
            const ::Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput(vecStick, FALSE);
            switch (idRule) {
                case ::Console::RULE_STICK_UP:
                    MoveTraitSelector(DIRECTION_UP);
                    break;
                case ::Console::RULE_STICK_DOWN:
                    MoveTraitSelector(DIRECTION_DOWN);
                    break;
                case ::Console::RULE_STICK_LEFT:
                    MoveTraitSelector(DIRECTION_LEFT);
                    break;
                case ::Console::RULE_STICK_RIGHT:
                    MoveTraitSelector(DIRECTION_RIGHT);
                    break;
            }
            break;
        }
        default: {
            const ::Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput(vecStick, FALSE);
            switch (idRule) {
                case ::Console::RULE_STICK_LEFT:
                    MoveSkillSelector(DIRECTION_LEFT);
                    break;
                case ::Console::RULE_STICK_RIGHT:
                    MoveSkillSelector(DIRECTION_RIGHT);
                    break;
            }
            break;
        }
    }
}

void PlayerSkillsGlobals::OnButton4(const UIID &, const int32)  // line 2566
{
    if (::Console::Screens::GetActionManager().GetActionForInput(6) == ACTION_CLEAR_SKILL) {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_CLEAR), cgtPSODefaults);
        UnassignSkillMessage tMessage;
        SigmaMemoryClear(&tMessage, sizeof(tMessage));
        tMessage.nSlot = GetSelectedSlot();
        MessageSendToServer(CMSG_UNASSIGN_SKILL, &tMessage, sizeof(tMessage), ptPlayer);
    }
}

void PlayerSkillsGlobals::OnButtonL2(const UIID &, const int32)  // line 2584
{
    if (!::Console::ControllerUtils::IsButtonDown(ptPlayer->nControllerIndex, 7)) {
        ScrollDetailInfo(m_eSelectionMode, -4.0f);
    }
}

void PlayerSkillsGlobals::OnButtonR2(const UIID &, const int32)  // line 2598
{
    if (!::Console::ControllerUtils::IsButtonDown(ptPlayer->nControllerIndex, 6)) {
        ScrollDetailInfo(m_eSelectionMode, 4.0f);
    }
}

void PlayerSkillsGlobals::OnDPadLeft(const int32)  // line 2612
{
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        MoveTraitSelector(DIRECTION_LEFT);
    } else {
        MoveSkillSelector(DIRECTION_LEFT);
    }
}

void PlayerSkillsGlobals::OnDPadRight(const int32)  // line 2628
{
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        MoveTraitSelector(DIRECTION_RIGHT);
    } else {
        MoveSkillSelector(DIRECTION_RIGHT);
    }
}

void PlayerSkillsGlobals::OnDPadUp(const int32)  // line 2644
{
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        MoveTraitSelector(DIRECTION_UP);
    }
}

void PlayerSkillsGlobals::OnDPadDown(const int32)  // line 2656
{
    if (m_eSelectionMode == TRAIT_SELECTION_MODE) {
        MoveTraitSelector(DIRECTION_DOWN);
    }
}

void PlayerSkillsGlobals::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)  // line 2668
{
    if (idIcon == GetUIIDNone()) {
        return;
    }

    if (IconIsKanaiPowers(idIcon)) {
        if (bSelected) {
            m_nCurrentlyViewedSkillSlot = KANAI_SLOT;
            ImageTextureFrame tTex("UI:Console_Ring_Cursor");
            UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RING_CURSOR).SetTextureFrame(tTex);
            UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.SkillLoadoutRingGlow")).SetActive(TRUE);
            UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.PassiveLoadoutRingGlow")).SetActive(FALSE);
            RefreshUI();
            ResetDetailScrollPositions();
        }
        UIID idButton = UIIDCreateFromParent(idIcon, ".Selection");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);
    } else {
        if (IconIsSkill(idIcon)) {
            if (bSelected) {
                RefreshSkillsList();
                m_nCurrentlyViewedSkillSlot = SlotFromIcon(idIcon);
                const SNO snoAssignedSkill = GetSkillForSlot(*ptPlayer, SlotFromIcon(idIcon));
                if ((Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge()) && snoAssignedSkill != -1) {
                    for (int32 i = 0; i < 6; i++) {
                        if (GetIndexOfSkill(i, snoAssignedSkill) != -1) {
                            m_nCurrentlyViewedSkillSlot = i;
                            break;
                        }
                    }
                }
                const int32 nSkillIndex = (snoAssignedSkill != -1) ? GetIndexOfSkill(m_nCurrentlyViewedSkillSlot, snoAssignedSkill) : 0;
                m_tSkillSelector.SetSelectedIndex(Math::Max(nSkillIndex, 0), FALSE);
                ImageTextureFrame tTex("UI:Console_Ring_Cursor");
                UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RING_CURSOR).SetTextureFrame(tTex);
                UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.SkillLoadoutRingGlow")).SetActive(TRUE);
                UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.PassiveLoadoutRingGlow")).SetActive(FALSE);
                RefreshUI();
            }
        } else if (bSelected) {
            ImageTextureFrame tTex("UI:Console_Ring_Cursor2");
            UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_RING_CURSOR).SetTextureFrame(tTex);
            UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.SkillLoadoutRingGlow")).SetActive(FALSE);
            UI_GetWindowFromID(UIIDCreateFromParent(UIDLG_SKILLS_CONSOLE, ".LayoutRoot.SkillLoadout.PassiveLoadoutRingGlow")).SetActive(TRUE);
            m_nCurrentlyViewedSkillSlot = SlotFromIcon(idIcon);
            RefreshTraitsList();
            RefreshUI();
        }
        UIText & tUnlockLevelText = UI_GetTextFromID(UIIDCreateFromParent(idIcon, ".LayoutRoot.Locked.LockedFade.LevelRequirementNumber"));
        tUnlockLevelText.SetTextColor(!bSelected ? crgbaGray : crgbaWhite);
        UIID idSparkle = UIIDCreateFromParent(idIcon, ".LayoutRoot.FX");
        UI_GetWindowFromID(idSparkle).SetActive(m_eSelectionMode == SLOT_SELECTION_MODE && bSelected);
        UIID idButton = UIIDCreateFromParent(idIcon, ".LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

BOOL PlayerSkillsGlobals::SkillSelectionPredicate(const int32 nIndex)  // line 2785
{
    return nIndex >= -1 && nIndex < GetNumSkills(m_nCurrentlyViewedSkillSlot);
}

// NON_MATCHING: 8 of 128 instructions. The sxtw of nIndex is shared by both tSkillEntry
// accesses here; the original folds it into an smaddl at the level compare and redoes it
// for the SetBit index. tACDPlayer is const per DWARF; that alone does not change it.
#line 2794
void PlayerSkillsGlobals::OnSkillSelectionChanged(
    const UIID & idSkill,
    const int32 nIndex,
    const BOOL bSelected,
    const BOOL bTriggeredByInput)
{
    if (idSkill == GetUIIDNone()) {
        return;
    }
    if (bSelected) {



        const int32 nOldSelectedSkillIndex = m_nSelectedSkillIndex;
        m_nSelectedSkillIndex = nIndex;

        AnimateHighlight();
        if (nIndex != -1) {



            const SkillListEntry & tSkillEntry = GetSkill(m_nCurrentlyViewedSkillSlot, nIndex);
            const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);

            const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            const int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nLocalPlayerIndex != -1 && nLevel >= tSkillEntry.nLevel) {



                SetBit(arExploredSkills[nLocalPlayerIndex].tExploredSkills, SkillRuneIndexToBit(tSkillEntry.nIndex, -1));
            }
            UpdateSkillItemUI(nIndex, FALSE);
        }
        if (nOldSelectedSkillIndex != -1) {
            UpdateSkillItemUI(nOldSelectedSkillIndex, FALSE);
        }

#line 2840
        UpdateSkillInfoUI();
        UpdateRunesListUI();
        UpdateRuneInfoUI();

        ResetDetailScrollPositions();
    }

    PlaySelectionSound(idSkill, bSelected);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

BOOL PlayerSkillsGlobals::RuneSelectionPredicate(const int32 nIndex)  // line 2855
{
    return nIndex >= -1 && nIndex < GetNumRunes();
}

void PlayerSkillsGlobals::OnRuneSelectionChanged(const UIID & idRune, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)  // line 2864
{
    if (bSelected) {
        m_nSelectedRuneIndex = nIndex;
        if (nIndex != -1 && m_nSelectedSkillIndex != -1) {
            const SkillListEntry & tSkillEntry = GetSkill(m_nCurrentlyViewedSkillSlot, m_nSelectedSkillIndex);
            const RuneEntry & tRuneEntry = tSkillEntry.tRunes[nIndex];
            const int32 nBit = SkillRuneIndexToBit(tSkillEntry.nIndex, tRuneEntry.eRuneType);
            const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            const int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nLevel >= tSkillEntry.nLevel) {
                const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);
                if (nLocalPlayerIndex != -1) {
                    SetBit(arExploredSkills[nLocalPlayerIndex].tExploredSkills, nBit);
                }
            }
            UpdateSkillItemUI(m_nSelectedSkillIndex, FALSE);
            UpdateRunesListItemUI(nIndex);
        }
        AnimateHighlight();
    }
    if (idRune != GetUIIDNone()) {
        UIID idHighlight;
        idHighlight.SetFromParent(idRune, "LayoutRoot.SelectedGlow");
        UI_GetWindowFromID(idHighlight).SetActive(bSelected);
        UIID idEffect;
        idEffect.SetFromParent(idRune, "LayoutRoot.SelectFX");
        UI_GetWindowFromID(idEffect).SetActive(bSelected);
        PlaySelectionSound(idRune, nIndex != -1 && bSelected);
    }
    UpdateRuneInfoUI();
    ResetDetailScrollPositions();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

BOOL PlayerSkillsGlobals::TraitSelectionPredicate(const int32 nIndex)  // line 2922
{
    return nIndex >= -1 && nIndex < GetNumTraits();
}

void PlayerSkillsGlobals::OnTraitSelectionChanged(const UIID & idTrait, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)  // line 2931
{
    if (idTrait == GetUIIDNone()) {
        return;
    }
    if (bSelected) {
        const int32 nOldSelectedTraitIndex = m_nSelectedTraitIndex;
        m_nSelectedTraitIndex = nIndex;
        AnimateHighlight();
        if (nIndex != -1) {
            const SkillListEntry & tSkillEntry = GetTrait(nIndex);
            const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            const int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nLevel >= tSkillEntry.nLevel) {
                const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);
                if (nLocalPlayerIndex != -1) {
                    SetBit(arExploredSkills[nLocalPlayerIndex].tExploredTraits, tSkillEntry.nIndex);
                }
            }
            UpdateSkillItemUI(nIndex, TRUE);
        }
        if (nOldSelectedTraitIndex != -1) {
            UpdateSkillItemUI(nOldSelectedTraitIndex, TRUE);
        }
        UpdateTraitInfoUI();
        ResetDetailScrollPositions();
    }
    PlaySelectionSound(idTrait, bSelected);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerSkillsGlobals::CacheRingSelection()  // line 2984
{
    m_nCachedRingSelection = GetSelectedSlot();
}

BOOL PlayerSkillsGlobals::RestoreCachedRingSelection()  // line 2993
{
    if (m_nCachedRingSelection == -1) {
        return FALSE;
    }
    UIID idSkill;
    if (SlotIsSkill(m_nCachedRingSelection)) {
        idSkill = GetUIIDForSkillSlot(m_nCachedRingSelection, UIWND_SKILLS_CONSOLE_RING);
    } else if (SlotIsPassive(m_nCachedRingSelection)) {
        idSkill = GetUIIDForTraitSlot(GetTraitSlotNumber(m_nCachedRingSelection), UIWND_SKILLS_CONSOLE_RING);
    } else {
        idSkill = UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON;
    }
    tRingUI.SelectControl(idSkill, IconSelectionChanged);
    m_nCachedRingSelection = -1;
    return TRUE;
}

BOOL PlayerSkillsGlobals::CanAssignSkill(const SkillListEntry * ptSkill) const  // line 3022
{
    if (ptSkill == NULL) {
        return FALSE;
    }
    const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD == NULL || !PlayerCanChangeSkillorTrait(*ptACD, FALSE)) {
        return FALSE;
    }
    if (IsWaitingForCooldown(GetSelectedSlot())) {
        return FALSE;
    }
    if (GameIsWeeklyChallenge() && !ChallengeRift::IsValidChallengeRiftSkill(ptSkill->snoPower)) {
        return FALSE;
    }
    return PlayerMeetsSkillLevelRequirement(ptPlayer->idPlayerACD, ptSkill);
}

BOOL PlayerSkillsGlobals::CanAssignTrait(const SkillListEntry * ptSkill) const  // line 3052
{
    if (ptSkill == NULL) {
        return FALSE;
    }
    const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD == NULL || !PlayerCanChangeSkillorTrait(*ptACD, FALSE)) {
        return FALSE;
    }
    for (int32 i = 0; i < 4; i++) {
        if (GetTraitForSlot(*ptPlayer, i) == ptSkill->snoPower) {
            return FALSE;
        }
    }
    return PlayerMeetsSkillLevelRequirement(ptPlayer->idPlayerACD, ptSkill);
}

BOOL PlayerSkillsGlobals::CanAssignRune(const RuneEntry * ptRune) const  // line 3081
{
    if (ptRune == NULL) {
        return FALSE;
    }
    const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD == NULL || !PlayerCanChangeSkillorTrait(*ptACD, FALSE)) {
        return FALSE;
    }
    if (IsWaitingForCooldown(GetSelectedSlot())) {
        return FALSE;
    }
    if (GameIsWeeklyChallenge()) {
        const SNO snoSkill = GetSkill(m_nCurrentlyViewedSkillSlot, m_nSelectedSkillIndex).snoPower;
        if (ptRune->eRuneType != ChallengeRift::GetRuneTypeForSkill(snoSkill)) {
            return FALSE;
        }
    }
    return PlayerMeetsRuneLevelRequirement(ptPlayer->idPlayerACD, ptRune);
}

BOOL PlayerSkillsGlobals::IsInTieredRift() const  // line 3114
{
    return UIUtils::Console::PlayerIsInActiveTieredRift(ptPlayer);
}


void InitializeForGame()  // line 3122
{
    sgptSkillsGlobals = new PlayerSkillsGlobals();
    Unk19b630 * ptKanaisPowers = (Unk19b630 *)operator new(sizeof(Unk19b630));
    sub_710019b630(ptKanaisPowers, UIWND_SKILLS_CONSOLE_KANAIS_POWERS);
    s_unk_186dc48 = ptKanaisPowers;
}

void DisposeForGame()  // line 3130
{
    delete s_unk_186dc48;
    s_unk_186dc48 = NULL;
    delete sgptSkillsGlobals;
    sgptSkillsGlobals = NULL;
}

#line 3138
void InitializeForPlayer(
    const int32 nPlayerIndex)
{
    const int32 nIndex = PlayerIndexToLocalPlayerIndex(nPlayerIndex);
    if (nIndex == -1) {
        return;
    }


    const ACDID idACDPlayer = LocalPlayersACDGetByIndex(nIndex);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (idACDPlayer == ACDID_INVALID || ptPlayer == NULL) {
        return;
    }


    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (ptACD == NULL) {
        return;
    }
    SigmaMemoryClear(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredSkills, sizeof(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredSkills));
    SigmaMemoryClear(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredTraits, sizeof(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredTraits));
    sgptSkillsGlobals->arExploredSkills[nIndex].bHasNewSkills = FALSE;
    SigmaMemoryClear(sgptSkillsGlobals->arSnapshotSkills[nIndex].tExploredSkills, sizeof(sgptSkillsGlobals->arSnapshotSkills[nIndex].tExploredSkills));
    SigmaMemoryClear(sgptSkillsGlobals->arSnapshotSkills[nIndex].tExploredTraits, sizeof(sgptSkillsGlobals->arSnapshotSkills[nIndex].tExploredTraits));
    sgptSkillsGlobals->arSnapshotSkills[nIndex].bHasNewSkills = FALSE;

#line 3168
    const BOOL bNewSkillKit = sgptSkillsGlobals->arExploredSkills[nIndex].bNewSkillKit || (sgtAppGlobals.dwDebugFlags2 & (1 << 12)) != 0 /* debug flag name not in the DWARF */;
    BOOL bHasNewSkillsInSkillKit = FALSE;

    const SNO snoSkillKit = ptACD->GetSkillKit();

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    const int32 nLevel = ptPlayer->nCurrentLevel;
    for (int32 i = 0; i < 6; i++) {


        const SkillCategory eCategory = UISkills::GetSkillCategoryForSlot(i, ptPlayer->idPlayerACD);

        const int32 nTotalSkillCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        int32 nCount = 0;
        for (int32 j = 0; j < nTotalSkillCount; j++) {
            ActiveSkillEntry & tSkill = pdefSkillKit->arActiveSkillEntries[j];
            if (tSkill.snoPower != -1 && tSkill.eCategoryNew == eCategory && PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower))) {

#line 3190
                const int32 nSkillLevel = tSkill.nMasteryReq;
                const int32 nSkillIndex = i * MAX_SKILLS_PER_CATEGORY_CONSOLE + nCount;
                if (nLevel >= nSkillLevel) {
                    if (!bNewSkillKit || !tSkill.bNew) {
                        SetBit(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredSkills, SkillRuneIndexToBit(nSkillIndex, -1));
                    } else {
                        bHasNewSkillsInSkillKit = TRUE;
                    }

#line 3203
                    for (int32 nRune = 0; nRune <= MAX_RUNE_RANKS_CONSOLE; nRune++) {

                        const RuneType eRuneType = nRune - 1;
                        const int32 nRuneLevel = tSkill.GetRuneLevelRequirement(eRuneType);
                        if (nLevel >= nRuneLevel) {
                            if (bNewSkillKit && tSkill.bRuneNew[nRune]) {
                                bHasNewSkillsInSkillKit = TRUE;
                            } else {
                                SetBit(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredSkills, SkillRuneIndexToBit(nSkillIndex, eRuneType));
                            }
                        }
                    }
                }
                nCount++;
            }
        }
    }

#line 3228
    int32 nTraitCount = 0;
    int32 nTotalTraitCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nTotalTraitCount; i++) {
        TraitEntry & tSkill = pdefSkillKit->arTraitEntries[i];
        if (tSkill.snoPower != -1 && PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower))) {
            if (nTraitCount < 20) {
                if (nLevel >= tSkill.nMasteryReq) {
                    if (bNewSkillKit && tSkill.bNew) {
                        bHasNewSkillsInSkillKit = TRUE;
                    } else {
                        SetBit(sgptSkillsGlobals->arExploredSkills[nIndex].tExploredTraits, nTraitCount);
                    }
                }
                nTraitCount++;
            }
        }
    }
    if (bHasNewSkillsInSkillKit) {

#line 3264
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_NEW_SKILLS), ptPlayer, TRUE);
    }
    sgptSkillsGlobals->arExploredSkills[nIndex].bNewSkillKit = FALSE;
}

void SetNewSkillKit(const int32 nControllerIndex)  // line 3272
{
    if (nControllerIndex >= 0 && nControllerIndex < 4) {
        sgptSkillsGlobals->arExploredSkills[nControllerIndex].bNewSkillKit = TRUE;
    }
}

BOOL IsActive()  // line 3286
{
    return UI_GetWindowFromID(UIDLG_SKILLS_CONSOLE).IsActive();
}

void OpenForController(int32 nControllerOpening)  // line 3314
{
    if (!IsActive() && nControllerOpening != -1) {
        Player * ptPlayerOpening = PlayerGetByControllerIndex(nControllerOpening);
        if (ptPlayerOpening == NULL) {
            return;
        }
        const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayerOpening->nPlayerIndex);
        if (nLocalPlayerIndex == -1) {
            return;
        }
        sgptSkillsGlobals->m_fCurrentlyOpening = TRUE;
        sgptSkillsGlobals->OnOpen(ptPlayerOpening);
        sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].bHasNewSkills = FALSE;
        SigmaMemoryCopy(sgptSkillsGlobals->arSnapshotSkills[nLocalPlayerIndex].tExploredSkills, sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredSkills, sizeof(sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredSkills));
        SigmaMemoryCopy(sgptSkillsGlobals->arSnapshotSkills[nLocalPlayerIndex].tExploredTraits, sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredTraits, sizeof(sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredTraits));
        ::Console::UIRActorManager::SetActive(TRUE);
        UpdateCharacterInfo(*ptPlayerOpening, FALSE);
        ::Console::Screens::ShowModalDialog(nControllerOpening, UIDLG_SKILLS_CONSOLE, FALSE, GetUIIDNone());
        UI_SetControllerFocus(UIWND_SKILLS_CONSOLE_RING);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        UIInventory::Console::UpdateNavBarTabState(ptPlayerOpening, UIDLG_SKILLS_CONSOLE, "LayoutRoot.Navigation", 1);
        if (!sgptSkillsGlobals->RestoreCachedRingSelection()) {
            InitRingSelection();
        }
        Refresh(ptPlayerOpening);
        sgptSkillsGlobals->m_fCurrentlyOpening = FALSE;
        gtTutorialMgr.ShowTutorial(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_POWER_DETAILS), NULL, TRUE);
    }
}

static void sStopSelectorStoryboards(::Console::RadioSelectorWrapper & tSelector)  // line 3373
{
    const UIIDArray & aridOptions = tSelector.GetOptionControlIDs();
    for (uint32 i = 0; i < aridOptions.size(); i++) {
        UIStoryboard & tHighlightStoryboard = UI_GetStoryboardFromID(UIIDCreateFromParent(aridOptions[i], ".SkillListHighlight"));
        if (tHighlightStoryboard.IsPlaying()) {
            tHighlightStoryboard.Stop();
            tHighlightStoryboard.Reset();
        }
        UIStoryboard & tSelectionStoryboard = UI_GetStoryboardFromID(UIIDCreateFromParent(aridOptions[i], ".SkillListSelection"));
        if (tSelectionStoryboard.IsPlaying()) {
            tSelectionStoryboard.Stop();
            tSelectionStoryboard.SkipToEnd();
        }
    }
}

void PlayerSkillsGlobals::StopAllStoryboards()  // line 3399
{
    UIIDArray aridTargets;
    tRingUI.GetTargetControlIDs(aridTargets);
    for (uint32 i = 0; i < aridTargets.size(); i++) {
        UIID idStoryboard = UIIDCreateFromParent(aridTargets[i], ".LoadoutEquipSkill");
        if (UI_TestID(idStoryboard)) {
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Reset();
        }
    }
    sStopSelectorStoryboards(m_tSkillSelector);
    sStopSelectorStoryboards(m_tRuneSelector);
    sStopSelectorStoryboards(m_tTraitSelector);
}

void Close()  // line 3424
{
    sgptSkillsGlobals->StopAllStoryboards();
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->CloseSelectedSlot();
    }
    if (IsActive()) {
        ::Console::Screens::CloseModalDialog(UIDLG_SKILLS_CONSOLE);
    }
    ::Console::UIRActorManager::SetActive(FALSE);
}

void OnUpdate(const UIID &)  // line 3448
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnUpdate();
    }
}

void Refresh(const Player * ptPlayer)  // line 3461
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->ptPlayer == ptPlayer && sgptSkillsGlobals->ptPlayer != NULL) {
        sgptSkillsGlobals->Refresh();
    }
}

void OnInventoryChanged(const ActorLocation &, const ActorLocation & tNewLoc, BOOL)  // line 3473
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        if (tNewLoc.tInvLocation.idACDContainer == sgptSkillsGlobals->ptPlayer->idPlayerACD) {
            ::Console::UIRActorManager::ForceRActorRecreate();
            UpdateCharacterInfo(*sgptSkillsGlobals->ptPlayer, TRUE);
        }
    }
}

void OnPlayerLeveled(const Player * ptPlayer)  // line 3499
{
    if (sgptSkillsGlobals == NULL) {
        return;
    }
    if (sgptSkillsGlobals->IsActive() && sgptSkillsGlobals->ptPlayer == ptPlayer) {
        return;
    }
    const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);
    if (nLocalPlayerIndex == -1) {
        return;
    }
    BOOL bNewSkill = FALSE;
    for (uint32 nSlot = 0; nSlot < 6; nSlot++) {
        if (HasNewSkillOrRune(nSlot, -1)) {
            bNewSkill = TRUE;
            break;
        }
    }
    if (bNewSkill || HasNewTrait(ptPlayer->nCurrentLevel)) {
        sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].bHasNewSkills = TRUE;
    }
}

BOOL HasNewSkillBlinker(const Player * ptPlayer)  // line 3533
{
    const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayer->nPlayerIndex);
    if (nLocalPlayerIndex == -1) {
        return FALSE;
    }
    if (sgptSkillsGlobals == NULL) {
        return FALSE;
    }
    return sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].bHasNewSkills != FALSE;
}

void InitRingSelection()  // line 3549
{
    {
        UIID idSkill = GetUIIDForSkillSlot(0, UIWND_SKILLS_CONSOLE_RING);
        sgptSkillsGlobals->tRingUI.SelectControl(idSkill, IconSelectionChanged);
    }
}

inline int32 GetTraitSlotNumber(const int32 nSlot)  // line 3562; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return (nSlot == -1) ? -1 : nSlot - 6;
}

void ShowCategoryName(const UIID & id, const int32 nSlot, const SkillCategory eCategory)  // line 3571
{
    CRefString sTitle;
    if (SlotIsSkill(nSlot)) {
        if (eCategory == -1) {
            sTitle = "[PH] No Category Found";
        } else {
            sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SKILLS_UI), UISkills::GetSkillCategoryLabel(eCategory));
        }
    } else if (SlotIsPassive(nSlot)) {
        sTitle = LocalizedSprintf(StringListFetch("ConsoleSkills:PassiveSlotName").str(), GetRomanNumeralForTraitSlot(nSlot), NULL);
    }
    UI_GetTextFromID(id).SetText(sTitle.str(), FALSE);
}

void ShowCategoryDescription(const UIID & id, const Player * ptPlayer, const int32 nSlot, const SkillCategory eCategory)  // line 3604
{
    CRefString sText;
    const ActorCommonData * ptACDPlayer = NULL;
    int32 nLevel = -1;
    if (ptPlayer != NULL) {
        ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer == NULL) {
            return;
        }
        nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    }
    if (SlotIsSkill(nSlot)) {
        if (eCategory == -1) {
            sText = "[PH] Unknown Category";
        } else {
            sText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_SKILLS), CRefString().Format("%s_Description", UISkills::GetSkillCategoryLabel(eCategory).str()));
        }
        if (nLevel != -1 && nLevel < cgnActiveSkillSlotLevelReq[nSlot]) {
            CRefString sUnlock = LocalizedSprintf(StringListFetch("ConsoleSkills:SlotUnlockLevel").str(), UnicodeText::FormatNumber(cgnActiveSkillSlotLevelReq[nSlot], TRUE, FNS_NORMAL, 0).str(), NULL);
            sText += "\n\n";
            sText += sUnlock;
        }
    } else {
        const int32 nTraitSlot = GetTraitSlotNumber(nSlot);
        sText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_SKILLS), CRefString().Format("Cat_Passive%d_Description", nTraitSlot + 1));
        if (nLevel != -1 && nLevel < cgnTraitSlotLevelReq[nTraitSlot]) {
            CRefString sUnlock = LocalizedSprintf(StringListFetch("ConsoleSkills:SlotUnlockLevel").str(), UnicodeText::FormatNumber(cgnTraitSlotLevelReq[nTraitSlot], TRUE, FNS_NORMAL, 0).str(), NULL);
            sText += "\n\n";
            sText += sUnlock;
        }
    }
    UI_GetTextFromID(id).SetText(sText.str(), FALSE);
}

LPCSTR GetRomanNumeralForTraitSlot(const int32 nSlot)  // line 3671
{
    static LPCSTR ROMAN_NUMERALS[] = { "I", "II", "III", "IV" };
    return ROMAN_NUMERALS[nSlot - 6];
}

int32 SkillRuneIndexToBit(const int32 nSkillIndex, const RuneType eRune)  // line 3693
{
    return nSkillIndex * (MAX_RUNE_RANKS_CONSOLE + 1) + eRune + 1;
}

BOOL EntryHasNewRune(const SkillListEntry & tCachedListEntry, const RuneEntry & tRuneEntry, const int32 nLocalPlayerIndex, const int32 nLevel)  // line 3702
{
    return tRuneEntry.nLevel <= nLevel &&
           !TestBit(sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredSkills, SkillRuneIndexToBit(tCachedListEntry.nIndex, tRuneEntry.eRuneType));
}

BOOL SkillHasNewRune(const SkillListEntry & tCachedListEntry, const int32 nLevel)  // line 3714
{
    const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(sgptSkillsGlobals->ptPlayer->nPlayerIndex);
    if (nLocalPlayerIndex == -1) {
        return FALSE;
    }
    for (int32 i = 0; i < MAX_RUNE_RANKS_CONSOLE; i++) {
        const RuneEntry & tRuneEntry = tCachedListEntry.tRunes[i];
        if (EntryHasNewRune(tCachedListEntry, tRuneEntry, nLocalPlayerIndex, nLevel)) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsSkillNew(const SkillListEntry & tCachedListEntry)  // line 3739
{
    const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(sgptSkillsGlobals->ptPlayer->nPlayerIndex);
    if (nLocalPlayerIndex == -1) {
        return FALSE;
    }
    return !TestBit(sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredSkills, SkillRuneIndexToBit(tCachedListEntry.nIndex, -1));
}

BOOL HasNewSkillOrRune(const int32 nSlot, const SNO snoPower)  // line 3754
{
    if (sgptSkillsGlobals->ptPlayer == NULL) {
        return FALSE;
    }
    ActorCommonData * ptACDPlayer = ACDTryToGet(sgptSkillsGlobals->ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return FALSE;
    }
    const int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    const SNO snoSkillKit = ptACDPlayer->GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    SkillList listSkills;
    GetSkillListForSlot(pdefSkillKit, ptACDPlayer->id, listSkills, nSlot);
    for (int32 i = 0; i < listSkills.size(); i++) {
        if (listSkills[i].nLevel <= nLevel && (snoPower == -1 || listSkills[i].snoPower == snoPower)) {
            if (IsSkillNew(listSkills[i]) || SkillHasNewRune(listSkills[i], nLevel)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL IsTraitNew(const SkillListEntry & tCachedListEntry, const int32 nLocalPlayerIndex, const int32 nLevel)  // line 3803
{
    return tCachedListEntry.nLevel <= nLevel &&
           !TestBit(sgptSkillsGlobals->arExploredSkills[nLocalPlayerIndex].tExploredTraits, tCachedListEntry.nIndex);
}

BOOL HasNewTrait(const int32 nLevel)  // line 3816
{
    if (sgptSkillsGlobals->ptPlayer == NULL) {
        return FALSE;
    }
    const int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(sgptSkillsGlobals->ptPlayer->nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(sgptSkillsGlobals->ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return FALSE;
    }
    const SNO snoSkillKit = ptACDPlayer->GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    SkillList listTraits;
    GetTraitList(pdefSkillKit, PlayerGetPlayerClass(ptACDPlayer), listTraits);
    const size_t nCount = listTraits.size();
    for (size_t i = 0; i < nCount; i++) {
        const SkillListEntry & tCachedListEntry = listTraits[i];
        if (IsTraitNew(tCachedListEntry, nLocalPlayerIndex, nLevel)) {
            return TRUE;
        }
    }
    return FALSE;
}

void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)  // line 3854
{
    sgptSkillsGlobals->OnIconSelectionChanged(idIcon, bSelected);
}

void OnSkillsHotkey(const UIID &, const int32 nControllerIndex)  // line 3864
{
    if (sgptSkillsGlobals != NULL && !::Console::Screens::IsScreenSetOpen() && PlayerGetByControllerIndex(nControllerIndex) != NULL) {
        ::Console::Screens::OpenToScreen(0, 1, nControllerIndex, FALSE);
    }
}

void OnLeftStick(const UIID & id, const int32 nControllerIndex)  // line 3879
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnLeftStick(id, nControllerIndex);
    }
}

void OnButton1(const UIID &, const int32)  // line 3891
{
    const ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(4);
    if (idAction != -1) {
        if (idAction != ACTION_OPEN_SLOT && UIUtils::Console::ShowTieredRiftMessage(sgptSkillsGlobals->ptPlayer)) {
            return;
        }
        switch (idAction) {
            case ACTION_OPEN_SLOT:
                sgptSkillsGlobals->OpenSelectedSlot();
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
                return;
            case ACTION_ASSIGN_SKILL:
                sgptSkillsGlobals->AssignSelectedSkill();
                return;
            case ACTION_ASSIGN_RUNE:
                sgptSkillsGlobals->AssignSelectedRune();
                return;
            case ACTION_ASSIGN_PASSIVE:
                sgptSkillsGlobals->AssignSelectedTrait();
                return;
        }
    }

    switch (sgptSkillsGlobals->GetSelectionMode()) {
        case PlayerSkillsGlobals::SKILL_SELECTION_MODE:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_INVALID_SELECTION), cgtPSODefaults);
            if (IsWaitingForCooldown(sgptSkillsGlobals->GetSelectedSlot())) {
                UIGame::DisplayError(GameErrorData(GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COOLDOWN), FALSE, sgptSkillsGlobals->ptPlayer);
                return;
            }
            break;
        case PlayerSkillsGlobals::TRAIT_SELECTION_MODE:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_INVALID_SELECTION), cgtPSODefaults);
            break;
        default:
            return;
    }
    {
        ActorCommonData * ptACD = ACDTryToGet(sgptSkillsGlobals->ptPlayer->idPlayerACD);
        if (ptACD == NULL || !PlayerCanChangeSkillorTrait(*ptACD, FALSE)) {
            UIGame::DisplayError(GameErrorData(GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_BOSS_ENCOUNTER), FALSE, sgptSkillsGlobals->ptPlayer);
        }
    }
}

void OnButton2(const UIID &, const int32)  // line 3963
{
    const ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(7);
    switch (idAction) {
        case ACTION_CLOSE:
            ::Console::Screens::CloseScreenSet(1, 0);
            break;
        case ACTION_CLOSE_SLOT:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
            sgptSkillsGlobals->GoBack();
            ResetDetailScrollPositions();
            break;
    }
}

void OnButton3(const UIID &, const int32 nControllerIndex)  // line 3983
{
    if (::Console::Screens::GetActionManager().GetActionForInput(5) == ACTION_SHOW_CHARACTER_DETAILS) {
        sgptSkillsGlobals->CacheRingSelection();
        ::Console::Screens::OpenToScreen(0x18, 0, nControllerIndex, FALSE);
        UICharacterDetails::Console::SetReturnScreen(1);
    }
}

void OnButton4(const UIID & id, const int32 nControllerIndex)  // line 4002
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnButton4(id, nControllerIndex);
    }
}

void OnButtonL2(const UIID & id, const int32 nControllerIndex)  // line 4015
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnButtonL2(id, nControllerIndex);
    }
}

void OnButtonR2(const UIID & id, const int32 nControllerIndex)  // line 4028
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnButtonR2(id, nControllerIndex);
    }
}

void OnDPadLeft(const UIID &, const int32 nControllerIndex)  // line 4041
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnDPadLeft(nControllerIndex);
    }
}

void OnDPadRight(const UIID &, const int32 nControllerIndex)  // line 4054
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnDPadRight(nControllerIndex);
    }
}

void OnDPadUp(const UIID &, const int32 nControllerIndex)  // line 4067
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnDPadUp(nControllerIndex);
    }
}

void OnDPadDown(const UIID &, const int32 nControllerIndex)  // line 4080
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnDPadDown(nControllerIndex);
    }
}
void UpdateCharacterInfo(const Player & tPlayer, BOOL bForceUpdate)  // line 4092
{
    const BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, 2, bForceUpdate, TRUE);
    if (bShouldCreateRActor) {
        VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
        UIInventory::Console::UpdatePlayerActorUI(UIRACTOR_CONSOLE_RING, *ptVisualInventory, tPlayer, 0x11000);
        SetUp3DCharacter(tPlayer);
    }
    CRefString sNumber;
    if (PlayerGetLocalPlayerCount() > 1 && PlayerIsLocal(&tPlayer)) {
        sNumber = CPlayerGetIndicatorString(tPlayer);
    }
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText(sNumber.str(), FALSE);
}

void SetUp3DCharacter(const Player & tPlayer)  // line 4117
{
    UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    const LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE,
                                     tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHero.SetLight(idLight);
    tHero.SetObserver(UIUtils::Console::GetObserverForPlayerClass(PlayerGetPlayerClass(&tPlayer)));
}

BOOL sSkillSelectionPredicate(const int32 nIndex)  // line 4135
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        return sgptSkillsGlobals->SkillSelectionPredicate(nIndex);
    }
    return FALSE;
}

void sSkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)  // line 4149
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnSkillSelectionChanged(idSkill, nIndex, bSelected, bTriggeredByInput);
    }
}

BOOL sRuneSelectionPredicate(const int32 nIndex)  // line 4164
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        return sgptSkillsGlobals->RuneSelectionPredicate(nIndex);
    }
    return FALSE;
}

void sRuneSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)  // line 4178
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnRuneSelectionChanged(idSkill, nIndex, bSelected, bTriggeredByInput);
    }
}

BOOL sTraitSelectionPredicate(const int32 nIndex)  // line 4193
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        return sgptSkillsGlobals->TraitSelectionPredicate(nIndex);
    }
    return FALSE;
}

void sTraitSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)  // line 4207
{
    if (sgptSkillsGlobals != NULL && sgptSkillsGlobals->IsActive()) {
        sgptSkillsGlobals->OnTraitSelectionChanged(idSkill, nIndex, bSelected, bTriggeredByInput);
    }
}

void UpdateLockedIndicator(const UIID & idParent, const BOOL bIsLocked)  // line 4222
{
    UIID idLockedIcon;
    idLockedIcon.SetFromParent(idParent, "LayoutRoot.LockedIcon");
    UI_GetWindowFromID(idLockedIcon).SetActive(bIsLocked);
}

void UpdateAssignedIndicator(const UIID & idParent, const AssignmentType eAssignment)  // line 4234
{
    UIID idEquippedIcon;
    idEquippedIcon.SetFromParent(idParent, "LayoutRoot.EquippedIcon");
    UI_GetWindowFromID(idEquippedIcon).SetActive(eAssignment != NOT_ASSIGNED);
    UIID idEquippedFrame;
    idEquippedFrame.SetFromParent(idParent, "LayoutRoot.EquippedFrame");
    UI_GetWindowFromID(idEquippedFrame).SetActive(eAssignment == ASSIGNED_IN_SLOT);
}

void UpdateSelectionHighlight(const UIID & idParent, const BOOL bIsSelected)  // line 4250
{
    UIID idHighlight;
    idHighlight.SetFromParent(idParent, "LayoutRoot.SelectedGlow");
    UI_GetWindowFromID(idHighlight).SetActive(bIsSelected);
    UIID idFX;
    idFX.SetFromParent(idParent, "LayoutRoot.SelectFX");
    UI_GetWindowFromID(idFX).SetActive(bIsSelected);
}

void UpdateNewItemIndicator(const UIID & idParent, const BOOL bIsNew)  // line 4266
{
    UIID idNewIndicator;
    idNewIndicator.SetFromParent(idParent, "LayoutRoot.NewStar");
    const BOOL bShow = bIsNew && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
    UI_GetWindowFromID(idNewIndicator).SetActive(bShow);
}

void PlayerSkillsGlobals::UpdateSkillRing()  // line 4279
{
    for (int32 i = 0; i < 6; i++) {
        UpdateSkillSlotIcon(i);
    }
    for (int32 i = 0; i < 4; i++) {
        UpdateTraitSlotIcon(i);
    }
    UpdateKanaisPowersSlotIcon();
}

void PlayerSkillsGlobals::UpdateSkillSlotIcon(const int32 nSlot)  // line 4298
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }
    const UIID idSkillIcon = GetUIIDForSkillSlot(nSlot, UIWND_SKILLS_CONSOLE_RING);
    const BOOL fRingEnabled = tRingUI.IsEnabled();
    const BOOL fSlotSelected = GetSelectedSlot() == nSlot;
    const SNO snoPower = GetSkillForSlot(*ptPlayer, nSlot);
    const BOOL fOnCooldown = !PowerIsCoolDownDone(*ptACDPlayer, snoPower);
    UISkills::Console::UpdateSkillSlotIcon(idSkillIcon, nSlot, ptPlayer, snoPower, fRingEnabled, fSlotSelected, fOnCooldown, FALSE);
}

void PlayerSkillsGlobals::UpdateTraitSlotIcon(const int32 nSlot)  // line 4322
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }
    const UIID idTraitSlot = GetUIIDForTraitSlot(nSlot, UIWND_SKILLS_CONSOLE_RING);
    const BOOL fRingEnabled = tRingUI.IsEnabled();
    const BOOL fSlotSelected = nSlot + 6 == GetSelectedSlot();
    const SNO snoPower = GetTraitForSlot(*ptPlayer, nSlot);
    const BOOL fOnCooldown = !PowerIsCoolDownDone(*ptACDPlayer, snoPower);
    UISkills::Console::UpdateTraitSlotIcon(idTraitSlot, nSlot, ptPlayer, snoPower, fRingEnabled, fSlotSelected, fOnCooldown, FALSE);
}

void PlayerSkillsGlobals::UpdateKanaisPowersSlotIcon()  // line 4343
{
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON).SetUserData((void *)KANAI_SLOT);
    const bool bLocked = GetCraftLevel(3, *ptPlayer) == 0;
    const bool bAssigned = ptPlayer->gbidLegendaryPowers[0] != -1 || ptPlayer->gbidLegendaryPowers[1] != -1 || ptPlayer->gbidLegendaryPowers[2] != -1;
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_LOCKED).SetActive(bLocked);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_UNASSIGNED).SetActive(!bLocked && !bAssigned);
    UI_GetWindowFromID(UIWND_SKILLS_CONSOLE_KANAIS_POWERS_RING_ICON_ASSIGNED).SetActive(!bLocked && bAssigned);
}

CRefString GetRuneTextureFrame(const RuneType eRuneType, const BOOL bEquipped)  // line 4361
{
    switch (eRuneType) {
        case 0:
            return bEquipped ? "UI:Console_SkillSelectionRune01_Equipped" : "UI:Console_SkillSelectionRune01";
        case 1:
            return bEquipped ? "UI:Console_SkillSelectionRune02_Equipped" : "UI:Console_SkillSelectionRune02";
        case 2:
            return bEquipped ? "UI:Console_SkillSelectionRune03_Equipped" : "UI:Console_SkillSelectionRune03";
        case 3:
            return bEquipped ? "UI:Console_SkillSelectionRune04_Equipped" : "UI:Console_SkillSelectionRune04";
        case 4:
            return bEquipped ? "UI:Console_SkillSelectionRune05_Equipped" : "UI:Console_SkillSelectionRune05";
        default:
            return bEquipped ? "UI:Console_SkillSelectionRune00_Selected" : "UI:Console_SkillSelectionRune00";
    }
}

inline BOOL CategorySkillFilter::IncludeSkill(const ActiveSkillEntry & tSkill) const  // line 4391; inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return tSkill.eCategoryNew == m_eCategory;
}

template <class FILTER>
void GetSkillList(const SkillKitDefinition * pdefSkillKit, SkillList & listSkills, const int32 nStartingIndex, FILTER & tFilter)  // line 4427
{
    listSkills.clear();
    const int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; i++) {
        const ActiveSkillEntry & tSkill = pdefSkillKit->arActiveSkillEntries[i];
        if (tSkill.snoPower != -1 && PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower)) && tFilter.IncludeSkill(tSkill)) {
            if (listSkills.size() > FILTER::MAX_SKILLS) {
                continue;
            }
            SkillListEntry tListEntry;
            tListEntry.nIndex = (int32)listSkills.size() + nStartingIndex;
            tListEntry.nLevel = tSkill.nMasteryReq;
            tListEntry.snoPower = tSkill.snoPower;
            tListEntry.eCategory = tSkill.eCategoryNew;
            for (int32 nRune = 0; nRune < MAX_RUNE_RANKS_CONSOLE; nRune++) {
                RuneEntry & tRune = tListEntry.tRunes[nRune];
                tRune.eRuneType = nRune;
                tRune.nLevel = tSkill.GetRuneLevelRequirement(nRune);
            }
            qsort(tListEntry.tRunes, MAX_RUNE_RANKS_CONSOLE, sizeof(RuneEntry), SortRunes);
            listSkills.push_back(tListEntry);
        }
    }
    blzext::vector_sort(listSkills, SortSkills);
}

void GetSkillListForSlot(const SkillKitDefinition * pdefSkillKit, const ACDID idPlayer, SkillList & listSkills, int32 nSlot)  // line 4470
{
    const SkillCategory eCategory = UISkills::GetSkillCategoryForSlot(nSlot, idPlayer);
    CategorySkillFilter tFilter(eCategory, PlayerGetPlayerClass(idPlayer));
    const int32 nStartingIndex = nSlot * MAX_SKILLS_PER_CATEGORY_CONSOLE;
    GetSkillList(pdefSkillKit, listSkills, nStartingIndex, tFilter);
}

void GetElectiveSkillList(const SkillKitDefinition * pdefSkillKit, const PlayerClass ePlayerClass, SkillList & listSkills)  // line 4486
{
    ElectiveSkillFilter tFilter(ePlayerClass);
    GetSkillList(pdefSkillKit, listSkills, 0, tFilter);
}

void GetTraitList(const SkillKitDefinition * pdefSkillKit, const PlayerClass ePlayerClass, SkillList & listTraits)  // line 4497
{
    listTraits.clear();
    const int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nCount; i++) {
        const TraitEntry & tSkill = pdefSkillKit->arTraitEntries[i];
        if (tSkill.snoPower != -1 && PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower))) {
            if (listTraits.size() >= 20) {
                continue;
            }
            SkillListEntry tListEntry;
            tListEntry.nIndex = listTraits.size();
            tListEntry.nLevel = tSkill.nMasteryReq;
            tListEntry.snoPower = tSkill.snoPower;
            listTraits.push_back(tListEntry);
        }
    }
    blzext::vector_sort(listTraits, SortSkills);
}

int32 SortRunes(const void * arg0, const void * arg1)  // line 4530
{
    const RuneEntry * ptEntry0 = (const RuneEntry *)arg0;
    const RuneEntry * ptEntry1 = (const RuneEntry *)arg1;
    return ptEntry0->nLevel - ptEntry1->nLevel;
}

int32 SortSkills(const void * arg0, const void * arg1)  // line 4542
{
    const SkillListEntry * ptEntry0 = (const SkillListEntry *)arg0;
    const SkillListEntry * ptEntry1 = (const SkillListEntry *)arg1;
    int32 nDiff = ptEntry0->nLevel - ptEntry1->nLevel;
    if (nDiff == 0) {
        CRefString sPower0;
        UISkills::GetName(ptEntry0->snoPower, sPower0);
        CRefString sPower1;
        UISkills::GetName(ptEntry1->snoPower, sPower1);
        nDiff = strcmp(sPower0.str(), sPower1.str());
    }
    return nDiff;
}

CRefString GetSkillName(const SNO snoPower)  // line 4567
{
    CRefString sSkillLabel = PowerGetNameLabel(snoPower);
    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sSkillLabel.str());
}

#line 4577
BOOL sGetSkillListEntry(
    const SNO snoPower,
    const PlayerClass ePlayerClass,
    const BOOL bIsTrait,
    SkillListEntry & tSkillListEntry)
{
    SNO snoSkillKit = SNO_NONE;

    const GameBalanceDefinition * pdefGB;
    const GBHandle hGB(GB_HEROS, PlayerClassToGBID(ePlayerClass));
    const HeroData * pHeroData = (const HeroData *)GBLockR(hGB, pdefGB, TRUE);
    if (pHeroData != NULL) {
        snoSkillKit = pHeroData->arSkillKit[0];

        GBUnlockR(pdefGB);
    }
    if (snoSkillKit != -1) {
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);

        SkillList tSkillList;
        if (bIsTrait) {

            GetTraitList(pdefSkillKit, ePlayerClass, tSkillList);
        } else {


            GetElectiveSkillList(pdefSkillKit, ePlayerClass, tSkillList);
        }

        for (int32 i = 0; i < tSkillList.size(); i++) {
            if (tSkillList[i].snoPower == snoPower) {
                tSkillListEntry = tSkillList[i];
                return TRUE;
            }
        }
    }
    return FALSE;


}

BOOL sGetSkillListEntry(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait, SkillListEntry & tSkillListEntry)  // line 4623
{
    return sGetSkillListEntry(snoPower, PlayerGetPlayerClass(&tACDPlayer), bIsTrait, tSkillListEntry);
}

int32 GetSkillLevel(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait)  // line 4635
{
    SkillListEntry tSkillListEntry;
    if (sGetSkillListEntry(snoPower, tACDPlayer, bIsTrait, tSkillListEntry)) {
        return tSkillListEntry.nLevel;
    }
    return 0;
}

SkillCategory GetSkillCategory(const SNO snoPower, const ActorCommonData & tACDPlayer, const BOOL bIsTrait)  // line 4653
{
    SkillListEntry tSkillListEntry;
    if (sGetSkillListEntry(snoPower, tACDPlayer, bIsTrait, tSkillListEntry)) {
        return tSkillListEntry.eCategory;
    }
    return -1;
}

SkillCategory GetSkillCategory(const SNO snoPower, const PlayerClass ePlayerClass, const BOOL bIsTrait)  // line 4671
{
    SkillListEntry tSkillListEntry;
    if (sGetSkillListEntry(snoPower, ePlayerClass, bIsTrait, tSkillListEntry)) {
        return tSkillListEntry.eCategory;
    }
    return -1;
}

void ShowSkillDescription(const UIID & idText, const SNO snoPower, ActorCommonData & tACDPlayer, const BOOL bIsTrait, const BOOL bShowDetails)  // line 4688
{
    CRefString sDescription;
    if (snoPower != -1) {
        const int32 nPlayerLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
        const int32 nSkillReqLevel = GetSkillLevel(snoPower, tACDPlayer, bIsTrait);
        if (nPlayerLevel < nSkillReqLevel) {
            sDescription += LocalizedSprintf(StringListFetch("ConsoleSkills:SkillUnlockLevel").str(), UnicodeText::FormatNumber(nSkillReqLevel, TRUE, FNS_NORMAL, 0).str(), NULL);
            sDescription += "\n\n";
        }
        const int32 nSkillLevel = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower));
        sDescription += UISkills::GetSkillDescriptionText(tACDPlayer, snoPower, nSkillLevel, bIsTrait, bIsTrait ? TRUE : bShowDetails);
        const GBID gbidItemTypeRequirement = PowerGeneralTagGetGBID(AutoPowerDefinitionR(snoPower), TAG_POWER_ITEM_TYPE_REQUIREMENT);
        if (gbidItemTypeRequirement == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_SHIELD) || gbidItemTypeRequirement == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_BOWCLASS)) {
            CRefString sRequirementString = StringListEntryReplace(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "RequiresItemType"), "{s1}", ItemTypeGetDescription(gbidItemTypeRequirement, TRUE, FALSE), TRUE);
            const BOOL bIsLocked = !PowerActorMeetsWeaponRequirement(tACDPlayer, AutoPowerDefinitionR(snoPower));
            if (bIsLocked) {
                Set_Text_Color(sRequirementString, 1);
            }
            sDescription += "\n";
            sDescription += sRequirementString;
            sDescription += "\n";
        }
    }
    UI_GetTextFromID(idText).SetText(sDescription.str(), FALSE);
}

int32 GetRuneLevel(const SNO snoPower, const RuneType eRune, const ActorCommonData & tACDPlayer)  // line 4737
{
    const SNO snoSkillKit = tACDPlayer.GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    SkillList tSkillList;
    GetElectiveSkillList(pdefSkillKit, PlayerGetPlayerClass(&tACDPlayer), tSkillList);
    for (int32 i = 0; i < tSkillList.size(); i++) {
        if (tSkillList[i].snoPower == snoPower) {
            for (int32 j = 0; j < MAX_RUNE_RANKS_CONSOLE; j++) {
                if (tSkillList[i].tRunes[j].eRuneType == eRune) {
                    return tSkillList[i].tRunes[j].nLevel;
                }
            }
        }
    }
    return 0;
}

void UpdatePlayerInfo(Player * ptPlayer)  // line 4769
{
    sgptSkillsGlobals->ptPlayer = ptPlayer;
}

void ShowRuneDescription(const UIID & idText, const SNO snoPower, const RuneType eRuneType, const BOOL bShowDetails)  // line 4777
{
    const ActorCommonData * ptACDPlayer = ACDTryToGet(sgptSkillsGlobals->ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL) {
        return;
    }
    CRefString sDescription;
    const int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    const int32 nRuneLevel = GetRuneLevel(snoPower, eRuneType, *ptACDPlayer);
    if (nPlayerLevel < nRuneLevel) {
        sDescription += LocalizedSprintf(StringListFetch("ConsoleSkills:RuneUnlockLevel").str(), UnicodeText::FormatNumber(nRuneLevel, TRUE, FNS_NORMAL, 0).str(), NULL);
        sDescription += "\n\n";
    }
    BOOL fFoundCombinedTooltipIgnored;
    sDescription += PlayerGetSpellRuneToolTip(*ptACDPlayer, snoPower, eRuneType, bShowDetails, FALSE, fFoundCombinedTooltipIgnored);
    UI_GetTextFromID(idText).SetText(sDescription, FALSE);
}

BOOL IsWaitingForCooldown(const int32 nSlot)  // line 4808
{
    BOOL bIsWaiting = FALSE;
    const ActorCommonData * ptACDPlayer = ACDTryToGet(sgptSkillsGlobals->ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL) {
        if (SlotIsSkill(nSlot)) {
            bIsWaiting = !PowerIsCoolDownDone(*ptACDPlayer, GetSkillForSlot(*sgptSkillsGlobals->ptPlayer, nSlot));
        } else {
            bIsWaiting = !PowerIsCoolDownDone(*ptACDPlayer, GetTraitForSlot(*sgptSkillsGlobals->ptPlayer, nSlot - 6));
        }
    }
    return bIsWaiting;
}

BOOL PlayerMeetsSkillLevelRequirement(const ACDID idPlayer, const SkillListEntry * ptSkill)  // line 4835
{
    if (idPlayer == ACDID_INVALID || ptSkill == NULL) {
        return FALSE;
    }
    const ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer == NULL) {
        return FALSE;
    }
    const int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    return ptSkill->nLevel <= nPlayerLevel;
}

BOOL PlayerMeetsRuneLevelRequirement(const ACDID idPlayer, const RuneEntry * ptRune)  // line 4857
{
    if (idPlayer == ACDID_INVALID || ptRune == NULL) {
        return FALSE;
    }
    const ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer == NULL) {
        return FALSE;
    }
    const int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    return ptRune->nLevel <= nPlayerLevel;
}

void PlaySelectionSound(const UIID & idParent, const BOOL bSelected)  // line 4879
{
    UIID idButton = UIIDCreateFromParent(idParent, ".LayoutRoot.Icon");
    UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, sgptSkillsGlobals->m_fCurrentlyOpening);
}

void AnimateSkillHighlight(const UIID & idItem)  // line 4890
{
    UIID idStoryboard;
    idStoryboard.SetFromParent(idItem, "SkillListHighlight");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    if (tStoryboard.IsPlaying()) {
        tStoryboard.SetOnEndCallback(NULL);
        tStoryboard.Stop();
    }
    tStoryboard.Play(FALSE, 0.0f);
}

void AnimateSkillSelection(const UIID & idItem)  // line 4910
{
    UIID idStoryboard;
    idStoryboard.SetFromParent(idItem, "SkillListSelection");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);
}

void sStoryboardEndCallback(const UIID & idCaller)  // line 4926
{
    const UIID & idWnd = UI_GetStoryboardFromID(idCaller).GetParent();
    UIID idCover = UIIDCreateFromParent(idWnd, ".LayoutRoot.PotionCooldownCover");
    UI_GetWindowFromID(idCover).SetActive(TRUE);
}

void AnimateEquippedSkillIcon(const UIID & idSlot)  // line 4935
{
    UIID idStoryboard;
    idStoryboard.SetFromParent(idSlot, "LoadoutEquipSkill");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.SetOnEndCallback(sStoryboardEndCallback);
    UIID idCover = UIIDCreateFromParent(idSlot, ".LayoutRoot.PotionCooldownCover");
    UI_GetWindowFromID(idCover).SetActive(FALSE);
    tStoryboard.Play(FALSE, 0.0f);
}

#line 4956
void UpdateSkillSlotIcon(
    const UIID & idSkillIcon,
    const int32 nSlot,
    const Player * ptPlayer,
    const SNO snoPower,
    const BOOL fRingEnabled,
    const BOOL fSlotSelected,
    const BOOL fOnCooldown,
    const BOOL fForInspect)
{
    UI_GetWindowFromID(idSkillIcon).SetUserData((void *)(intptr_t)nSlot);

#line 4971
    ActorCommonData * ptACDPlayer = NULL;
    if (ptPlayer != NULL) {


        ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer == NULL) {
            return;
        }
    }


    int32 nLevel = ptACDPlayer ? ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) : 0;
    BOOL fLocked = ptACDPlayer && nLevel < cgnActiveSkillSlotLevelReq[nSlot];
    BOOL fHasSkill = snoPower != -1;
    const SNO snoShader = (fRingEnabled || fSlotSelected) ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);


    UIID idFrame = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Frame");
    UI_GetWindowFromID(idFrame).SetShader(snoShader);


    UIID idUnlock = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Unlocked");
    UIWindow & tWndUnlock = UI_GetWindowFromID(idUnlock);
    tWndUnlock.SetActive(!fLocked && !fHasSkill);
    tWndUnlock.SetShader(snoShader);

    UIID idLock = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Locked");
    UIWindow & tWndLock = UI_GetWindowFromID(idLock);
    tWndLock.SetActive(fLocked);
    tWndLock.SetShader(snoShader);
    if (fLocked) {



        UIID idUnlockLevel = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Locked.LockedFade.LevelRequirementNumber");
        UIText & tUnlockLevelText = UI_GetTextFromID(idUnlockLevel);
        tUnlockLevelText.SetText(UnicodeText::FormatNumber(cgnActiveSkillSlotLevelReq[nSlot], TRUE, FNS_NORMAL, 0), FALSE);
        tUnlockLevelText.SetTextColor(crgbaGray);
    }


    UIID idIcon = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Icon");
    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
    fHasSkill = fHasSkill && !fLocked;
    tIcon.SetActive(fHasSkill);
    if (fHasSkill) {

        HotbarButtonData tHotbarButtonData;
        tHotbarButtonData.SetSkill(snoPower, -1);
        tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);
        tIcon.SetPlayer(ptPlayer);
        tIcon.UpdateTimer(fOnCooldown, TRUE);
    }
    tIcon.SetShader(snoShader);

#line 5029
    UIID idKeyBinding = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.KeyBinding");
    UIText & tTxtKeybinding = UI_GetTextFromID(idKeyBinding);
    if (!fForInspect) {
        tTxtKeybinding.SetActive(TRUE);

#line 5038
        tTxtKeybinding.SetControllerIndexOverride(ptPlayer->nControllerIndex);
        tTxtKeybinding.SetText(Controller::GetActionButtonIconName(nSlot, ptPlayer, TRUE), FALSE);
    } else {
        tTxtKeybinding.SetActive(FALSE);
    }
    int32 nEffectiveSlot = nSlot;
    if ((Prefs::GetGameplayPrefs().GetSkillElectiveMode() || GameIsWeeklyChallenge()) && snoPower != -1) {

#line 5050
        const SkillCategory eCategory = GetSkillCategory(snoPower, *ptACDPlayer, FALSE);
        nEffectiveSlot = UISkills::SkillCategoryGetSlot(eCategory);
    }


    UIID idNewIndicator = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.NewIndicator");
    const BOOL bShow = !fForInspect && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && HasNewSkillOrRune(nEffectiveSlot, -1);
    UI_GetWindowFromID(idNewIndicator).SetActive(bShow);


    UIID idSparkle = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.FX");
    UI_GetWindowFromID(idSparkle).SetActive(fSlotSelected && fRingEnabled);
}

BOOL IsSlotUnlocked(const ActorCommonData & tACDPlayer, const int32 nSlot)  // line 5067
{
    const int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
    const int32 nSlotLevelReq = SlotIsSkill(nSlot) ? cgnActiveSkillSlotLevelReq[nSlot] : cgnTraitSlotLevelReq[GetTraitSlotNumber(nSlot)];
    return nLevel >= nSlotLevelReq;
}


#line 5079
void UpdateTraitSlotIcon(
    const UIID & idTraitSlot,
    const int32 nSlot,
    const Player * ptPlayer,
    const SNO snoPower,
    const BOOL fRingEnabled,
    const BOOL fSlotSelected,
    const BOOL fOnCooldown,
    const BOOL fForInspect)
{

    const int32 nSkillSlot = nSlot + 6;

    const ActorCommonData * ptACDPlayer = NULL;
    if (ptPlayer != NULL) {


        ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer == NULL) {
            return;
        }
    }


    BOOL fLocked = ptACDPlayer && !IsSlotUnlocked(*ptACDPlayer, nSkillSlot);
    const BOOL fHasSkill = snoPower != -1;
    const SNO snoShader = (fRingEnabled || fSlotSelected) ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);
    UI_GetWindowFromID(idTraitSlot).SetUserData((void *)(intptr_t)nSkillSlot);

#line 5111
    UIID idFrame = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.Frame");
    UI_GetWindowFromID(idFrame).SetShader(snoShader);


    UIID idUnlock = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.Unlocked");
    UIWindow & tWndUnlock = UI_GetWindowFromID(idUnlock);
    tWndUnlock.SetActive(!fLocked && !fHasSkill);
    tWndUnlock.SetShader(snoShader);

    UIID idLock = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.Locked");
    UIWindow & tWndLock = UI_GetWindowFromID(idLock);
    tWndLock.SetActive(fLocked);
    tWndLock.SetShader(snoShader);
    if (fLocked) {



        UIID idUnlockLevel = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.Locked.LockedFade.LevelRequirementNumber");
        UIText & tUnlockLevelText = UI_GetTextFromID(idUnlockLevel);
        tUnlockLevelText.SetText(UnicodeText::FormatNumber(cgnTraitSlotLevelReq[nSlot], TRUE, FNS_NORMAL, 0), FALSE);
        tUnlockLevelText.SetTextColor(crgbaGray);
    }

    UIID idIcon = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.Icon");
    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
    tIcon.SetActive(fHasSkill && !fLocked);
    if (tIcon.IsItselfActive()) {


        HotbarButtonData tHotbarButtonData;
        tHotbarButtonData.SetSkill(snoPower, -1);
        tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);
        tIcon.UpdateTimer(fOnCooldown, FALSE);
    }
    tIcon.SetShader(snoShader);


    UIID idNewIndicator = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.NewIndicator");
    UIWindow & tWndNewIndicator = UI_GetWindowFromID(idNewIndicator);
    if (fForInspect)
    {
        tWndNewIndicator.SetActive(FALSE);
    }
    else
    {
        const int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        const BOOL bShow = !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !fLocked && HasNewTrait(nLevel);
        tWndNewIndicator.SetActive(bShow);
    }
    UIID idSparkle = UIIDCreateFromParent(idTraitSlot, ".LayoutRoot.FX");
    UI_GetWindowFromID(idSparkle).SetActive(fSlotSelected && fRingEnabled);
}

UIID GetUIIDForSkillSlot(const int32 nSlot, const UIID & idRing)  // line 5168
{
    return UIIDCreateFromParent(idRing, CRefString().Format(".LoadedSkillSlot%d", nSlot + 1));
}

UIID GetUIIDForTraitSlot(const int32 nSlot, const UIID & idRing)  // line 5177
{
    return UIIDCreateFromParent(idRing, CRefString().Format(".LoadedPassive%d", nSlot + 1));
}

BOOL SlotIsSkill(const int32 nSlot)  // line 5195
{
    return nSlot < 6;
}

BOOL SlotIsPassive(const int32 nSlot)  // line 5203
{
    return nSlot >= 6 && nSlot < 10;
}

BOOL IconIsSkill(const UIID & idIcon)  // line 5211
{
    const int32 nSlot = SlotFromIcon(idIcon);
    return SlotIsSkill(nSlot);
}

BOOL IconIsKanaiPowers(const UIID & idIcon)  // line 5233
{
    const int32 nSlot = SlotFromIcon(idIcon);
    return nSlot == KANAI_SLOT;
}

int32 SlotFromIcon(const UIID & idIcon)  // line 5244
{
    return (int32)(intptr_t)UI_GetWindowFromID(idIcon).GetUserData();
}

}  // namespace Console
}  // namespace UISkills
