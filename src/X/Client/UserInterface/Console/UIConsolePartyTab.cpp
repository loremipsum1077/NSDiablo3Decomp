#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleUIRActorManager.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../CGame.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of Online.h's nn::ldn constants.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/Trade.h"
#include "../../../Sigma/DataStructures/XGrowableArray.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTiming.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
typedef int32 LegendaryPowerSlotType;
typedef int32 SkillCategory;
// X/Common/Player.cpp
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
uint32 PlayersGetCount();
BOOL PlayerIsLocal(const Player * ptPlayer);
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);
// X/Common/Game.cpp
BOOL GameIsStartUp();
BOOL GameIsSeasonal();
// X/Common/Trade.cpp
TradeResponse TradeCanTradeWith(const Player & tFirst, ACDID idSecond);
// X/Common/ActorCommonData.cpp
ACDNetworkName ANNFromACD(ACDID idACD);
#include "../../Definitions/PreferencesDefinition.h"




namespace Prefs {
const GameplayPrefs & GetGameplayPrefs();
}  // namespace Prefs
// X/Common/Powers/PowersCommon.cpp
BOOL PowerIsDisplayed(AutoPowerDefinitionR pdefPower);
CRefString PowerGetRuneName(SNO snoPower, RuneType eRune);
RuneType PowerGetRuneTypeForACD(const ActorCommonData & tACD, SNO snoPower);
// X/Common/Attributes/FastAttribute.cpp
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);
// X/Common/GameBalance.cpp
CRefString GBIDToString(const GBID & gbid);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
// X/Common/Definitions/ActorDefinition.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
// X/Common/VisualInventory.cpp
struct VisualInventory;
VisualInventory * VisualInventoryGet(ACDID idACD);
GBID VisualInventoryGetCosmeticItemInSlot(ACDID idACD, int32 eSlot);
#include "../../../Common/Networking/GameMessage.h"




// X/Common/Networking/MessageHeaders.h. Only the size is known.



// X/Common/Networking/GameMessage.h. Only the members this file writes are declared.





// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/CPlayer.cpp
void CPlayerBeginInspect(int32 nPlayerIndex, Player * ptPlayer);
CRefString CPlayerGetIndicatorString(const Player & tPlayer);
// X/Client/CTrade.cpp
void CTradeTryStart(Player & tPlayer, Player & tOther);
// X/Client/EffectGroup.cpp
typedef int32 EffectGroupID;
void EffectGroupOutro(EffectGroupID idEffectGroup);
EffectGroupID EffectGroupCreateTracked(SNO snoEffectGroup, RActorID idRActorSource, RActorID idRActorTarget, int32 nUnused0, int32 nUnused1);
// X/Client/RActors/RActors.h:278 and :596. Only the members this file touches are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x410 - 0x04];  // 0x04 (invented name: idSScene .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/Input/Input.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
int32 UI_GetPrimaryUIController();
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
ConsoleModalNotificationManager & GetModalNotificationManager();
void UpdateButtonLegendText();
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
}  // namespace Screens
}  // namespace Console
// X/Client/Console/Online.cpp
namespace Console {
namespace Online {
BOOL PartyGetPartyMemberForPlayer(const Player * ptPlayer, PartyMember & tMember);
}  // namespace Online
}  // namespace Console
// X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp
namespace UIPlayerOptions {
namespace Console {
BOOL CanShowGamerCard(int32 nControllerIndex, ::Console::Online::PartyMember & tMember);
void ShowGamerCard(int32 nLocalUserIndex, OnlineUserId m_idFriend, char * nUserNickname, int32 nSizeOfNickName);
}  // namespace Console
}  // namespace UIPlayerOptions
// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GOLD_TRADE = 1,
};
typedef void (*StackSplitCallback)(const ACDID, const uint64);
void Open(ACDID idACD, uint64 uMin, uint64 uMax, StackSplitCallback pfnCallback, StackSplitType eType);
BOOL IsVisible();
void Close();
}  // namespace Console
}  // namespace UIStackSplit
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void DisplayErrorString(const CRefString & sError);
}  // namespace UIGame
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
namespace UIInventory {
namespace Console {
void UpdatePlayerActorUI(const UIID & idRActor, VisualInventory & tVisualInventory, const Player & tPlayer, uint32 dwFlags);
void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idParent, LPCSTR szNavBar, int32 nTab);
void OnPartyButton(const UIID & id, const int32 nControllerIndex, const InputButton eInputButton);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
void UpdateHeroNameAndLevel(const UIID & idParent, const Player & tPlayer);
void UpdateHeroStats(const UIID & idParent, const Player & tPlayer);
void DisplayPortrait(PlayerClass ePlayerClass, BOOL bIsFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortraitFrame);
BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
SkillCategory GetSkillCategoryForSlot(const int32 nSlot, const ACDID idPlayer);
BOOL GetName(SNO snoPower, CRefString & sName);
}  // namespace UISkills
// X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UISkills {
namespace Console {
UIID GetUIIDForSkillSlot(const int32 nSlot, const UIID & idParent);
UIID GetUIIDForTraitSlot(const int32 nSlot, const UIID & idParent);
BOOL IconIsKanaiPowers(const UIID & idIcon);
BOOL IconIsSkill(const UIID & idIcon);
int32 SlotFromIcon(const UIID & idIcon);
void UpdateSkillSlotIcon(const UIID & idIcon, const int32 nSlot, const Player * ptPlayer, SNO snoPower, BOOL bEnabled, BOOL bSelected, BOOL bUnused0, BOOL bUnused1);
void UpdateTraitSlotIcon(const UIID & idIcon, const int32 nSlot, const Player * ptPlayer, SNO snoPower, BOOL bEnabled, BOOL bSelected, BOOL bUnused0, BOOL bUnused1);
CRefString GetSkillName(SNO snoPower);
void ShowSkillDescription(const UIID & idText, SNO snoPower, ActorCommonData & tACD, BOOL bIsTrait, BOOL bShowDetails);
CRefString GetRuneTextureFrame(RuneType eRune, BOOL bUnused);
void UpdatePlayerInfo(Player * ptPlayer);
void ShowRuneDescription(const UIID & idText, SNO snoPower, RuneType eRune, BOOL bShowDetails);
void ShowCategoryName(const UIID & idText, int32 nSlot, SkillCategory eCategory);
void ShowCategoryDescription(const UIID & idText, const Player * ptPlayer, int32 nSlot, SkillCategory eCategory);
SkillCategory GetSkillCategory(SNO snoPower, const ActorCommonData & tACD, BOOL bUnused);
}  // namespace Console
}  // namespace UISkills
// X/Client/UserInterface/Interface/UIEquipLegendaryPowers.cpp
namespace UIEquipLegendaryPowers {
CRefString GetPowerTooltipText(GBID gbidPower, bool bUnused);
void ShowPowerIcon(const UIID & idVisual, GBID gbidPower, BOOL bUnused);
}  // namespace UIEquipLegendaryPowers
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_PARTY_CONSOLE;
extern UIID UIRACTOR_CONSOLE_RING;
extern UIID UITXT_CONSOLE_PLAYER_LABEL_RING;
extern UIID UIWND_PARTY_CONSOLE_CHARSELECT;
extern UIID UITXT_PARTY_CONSOLE_CHARSELECT_NAME;
extern UIID UIWND_PARTY_CONSOLE_EQUIPPED;
extern UIID UIWND_PARTY_CONSOLE_INVENTORY;
extern UIID UIWND_PARTY_CONSOLE_INVENTORY_CREST;
extern UIID UIWND_PARTY_CONSOLE_INVENTORY_PANE;
extern UIID UIWND_PARTY_CONSOLE_RING;
extern UIID UIWND_PARTY_CONSOLE_RING_CURSOR;
extern UIID UIWND_PARTY_CONSOLE_SKILLS;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_RING;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_RING_CURSOR;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_RING_GLOW;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_PASSIVE_RING_GLOW;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_VISUAL;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_VISUAL;
extern UIID UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_KANAIS_POWERS;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_ICON;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_RUNES;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_RUNE_ICON;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_SKILLS;
extern UIID UIWND_PARTY_SKILLS_CONSOLE_SKILL_ICON;
extern UIID UIBTN_PARTY_CONSOLE_RING_ARM_LEFT;
extern UIID UIBTN_PARTY_CONSOLE_RING_ARM_RIGHT;
extern UIID UIBTN_PARTY_CONSOLE_RING_BAG;
extern UIID UIBTN_PARTY_CONSOLE_RING_BRACERS;
extern UIID UIBTN_PARTY_CONSOLE_RING_FEET;
extern UIID UIBTN_PARTY_CONSOLE_RING_FINGER_LEFT;
extern UIID UIBTN_PARTY_CONSOLE_RING_FINGER_RIGHT;
extern UIID UIBTN_PARTY_CONSOLE_RING_HANDS;
extern UIID UIBTN_PARTY_CONSOLE_RING_HEAD;
extern UIID UIBTN_PARTY_CONSOLE_RING_LEGS;
extern UIID UIBTN_PARTY_CONSOLE_RING_NECK;
extern UIID UIBTN_PARTY_CONSOLE_RING_SHOULDER;
extern UIID UIBTN_PARTY_CONSOLE_RING_TORSO;
extern UIID UIBTN_PARTY_CONSOLE_RING_WAIST;
extern UIID UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR;
extern UIID UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY;
extern UIID UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON;
extern UIID UISCROLL_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO;
extern UIID UISCROLL_PARTY_SKILLS_CONSOLE_RUNE_INFO;
extern UIID UISCROLL_PARTY_SKILLS_CONSOLE_SKILL_INFO;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_DESCRIPTION;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_NAME;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_HEADER;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_DETAILS;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_NAME;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_NAME;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_DETAILS;
extern UIID UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_NAME;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_NAME;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_RUNE_INFO;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_RUNE_NAME;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_SKILL_INFO;
extern UIID UITXT_PARTY_SKILLS_CONSOLE_SKILL_NAME;


namespace UIParty {
namespace Console {

static const int32 KANAI_SLOT = 10;  // line 61

class InspectInvScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x2b8
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_LIST = 1,
    };
public:
    InspectInvScreen();
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void MarkRingUIAsDirty();
    virtual void Update();

private:
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);

    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)  // line 92
    {
        tRingWrapper.WrapUsingContainerControl(UIWND_PARTY_CONSOLE_RING, UIWND_PARTY_CONSOLE_RING_CURSOR, NULL, FALSE);
    }

    virtual UIFilterType GetDefaultRingSelection() const  // line 98
    {
        return FILTER_PLAYER_START;
    }

    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);

    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap)  // line 108
    {
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_ARM_LEFT, FILTER_MAIN_HAND);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_ARM_RIGHT, FILTER_OFF_HAND);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_BAG, FILTER_BAG);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_BRACERS, FILTER_WRIST);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_FEET, FILTER_FEET);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_FINGER_LEFT, FILTER_RING_LEFT);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_FINGER_RIGHT, FILTER_RING_RIGHT);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_HANDS, FILTER_HANDS);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_HEAD, FILTER_HEAD);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_LEGS, FILTER_LEGS);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_NECK, FILTER_NECK);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_SHOULDER, FILTER_SHOULDERS);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_TORSO, FILTER_CHEST);
        tFilterMap.SetAt_Tracked(UIBTN_PARTY_CONSOLE_RING_WAIST, FILTER_BELT);
    }

    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)  // line 127
    {
        return &m_tEquippedView;
    }

    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)  // line 134
    {
        return NULL;
    }

    virtual void OnOpen();
    virtual void OnClose();
    virtual void OnSetActive(BOOL bActive);
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);

    UIInventory::Console::EquippedItem m_tEquippedItemProvider;  // 0x200
    UIInventory::Console::UIInventoryDetailsView m_tEquippedView;  // 0x238
    BOOL m_fRingNeedsUpdating;  // 0x2b0
};
static_assert(sizeof(InspectInvScreen) == 0x2b8, "InspectInvScreen");

class InspectSkillsScreen {  // size 0x108
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_DETAIL_INFO = 1,
    };
public:
    InspectSkillsScreen();
    void HandleLeftStickInput(const int32 nControllerIndex);
    void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void Update();
    void Open();
    void Close();
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    void UpdateSkillSlotIcon(Player * ptPlayer, const int32 nSlot);
    void UpdateTraitSlotIcon(Player * ptPlayer, const int32 nSlot);
    void UpdateKanaisPowersSlotIcon(Player * ptPlayer);
    void UpdateSkillRing();
    void UpdateUIForSelectedSkill(const int32 nIconSlot, Player * ptPlayer);
    void UpdateUIForSelectedTrait(const int32 nIconSlot, Player * ptPlayer);

    int32 GetSelectedSlot() const  // line 214
    {
        const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
        if (idSelectedIcon != GetUIIDNone())
        {
            return UISkills::Console::SlotFromIcon(idSelectedIcon);
        }
        return -1;
    }

    ::Console::RingWrapper m_tRingUI;  // 0x00
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;  // 0x68
    SNOArray m_listSkills;  // 0x90
    SNOArray m_listTraits;  // 0xc8
    BOOL m_fCurrentlyOpening;  // 0x100
private:
    BOOL CanScrollDetails();
    void ResetDetailScrollPosition(const UIID & idScrollBox);
    void ResetDetailScrollPositions();
    void ScrollDetailInfo(const float nDelta);
};
static_assert(sizeof(InspectSkillsScreen) == 0x108, "InspectSkillsScreen");

class CharSelectScreen {  // size 0x40
    enum ButtonActions {
        ACTION_INSPECT = 1,
        ACTION_CLOSE = 2,
        ACTION_INSPECT_SKILLS = 3,
        ACTION_GIVE_GOLD = 4,
        ACTION_TRADE = 5,
        ACTION_VIEW_GAMERCARD = 6,
    };
public:
    CharSelectScreen();
    void HandleLeftStickInput(const int32 nControllerIndex);
    void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void Update();
    void Open();
    void Close();
protected:
    void sUpdateButtonLegend();
private:
    int32 m_nSelectedIndex;  // 0x00
    int32 m_nPlayerIndexMapping[3];  // 0x04
    ::Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x10
    EffectGroupID m_idHighlightEffect;  // 0x38

    void InitAnalogStickRules()  // line 290
    {
        const Blizzard::Time::Microsecond DEFAULT_STICK_DELAY = 200;
        m_tStickInputFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
        m_tStickInputFilter.SetMinimumInputDelay(DEFAULT_STICK_DELAY);
    }

    void sChangeSelection(const int32 * nDeltas);
    void SelectPrevious();
    void SelectNext();
    void OnSelectionChanged();
    void CleanupHighlightEffect();
    void OnCloseButton();
    void OnTradeButton(Player * ptPlayerLocal, Player * ptPlayerTarget);
    void OnGiveGold(Player * ptPlayerLocal, Player * ptPlayerTarget);
    void OnViewGamerCard(Player * ptPlayerLocal, Player * ptPlayerTarget);
};
static_assert(sizeof(CharSelectScreen) == 0x40, "CharSelectScreen");

class PartyTab {  // size 0x420 (UIParty::Console::InitializeForGame: new(0x420))
public:
    enum PartyTabMode {
        PTM_INVALID = -1,
        PTM_CHAR_SELECT = 0,
        PTM_INSPECT_INVENTORY = 1,
        PTM_INSPECT_SKILLS = 2,
    };

    PartyTab();
    void HandleLeftStickInput(const int32 nControllerIndex);
    void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void Update();
    void OnInspectAck(const int32 nPlayerIndex);
    void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
    void OpenForController(int32 nControllerIndex);
    void Close();
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);

    void SetDeferredMode(PartyTabMode eNewMode)  // line 369
    {
        m_eModeDeferred = eNewMode;
    }

    void SwitchMode(PartyTabMode eNewMode);

    BOOL IsActive() const  // line 377
    {
        return UI_GetWindowFromID(UIDLG_PARTY_CONSOLE).IsActive();
    }

    Player * GetPlayer()  // line 382
    {
        return m_ptPlayer;
    }

    void SetTargetPlayer(Player * ptPlayer)  // line 388
    {
        m_ptTargetPlayer = ptPlayer;
    }

    Player * GetTargetPlayer()  // line 394
    {
        return m_ptTargetPlayer;
    }

private:
    int32 m_nControllerIndex;  // 0x00
    Player * m_ptPlayer;  // 0x08
    Player * m_ptTargetPlayer;  // 0x10
    PartyTabMode m_eMode;  // 0x18
    PartyTabMode m_eModeDeferred;  // 0x1c
    CharSelectScreen m_tCharSelectScreen;  // 0x20
    InspectInvScreen m_tInspectInvScreen;  // 0x60
    InspectSkillsScreen m_tInspectSkillsScreen;  // 0x318
};
static_assert(sizeof(PartyTab) == 0x420, "PartyTab");

PartyTab * sgptPartyTabGlobals;  // line 412

void ShowPlayerModel(const Player & tPlayer, const UIID & idModel);
void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);
static BOOL sCanStillInspectTarget();
static void AttachLightToModel(UIRActor & tModel);

inline CharSelectScreen::CharSelectScreen()  // line 435
    : m_nSelectedIndex(0)
    , m_idHighlightEffect(-1)
{
    for (int32 nIndex = 0; nIndex < 3; nIndex++)
    {
        m_nPlayerIndexMapping[nIndex] = -1;
    }
    InitAnalogStickRules();
}

inline void CharSelectScreen::CleanupHighlightEffect()  // line 450
{
    EffectGroupOutro(m_idHighlightEffect);
    m_idHighlightEffect = -1;
}

void CharSelectScreen::OnSelectionChanged()  // line 460
{
    int32 nIndex = 0;
    CleanupHighlightEffect();

    for (const Player * ptPlayerToCheck = PlayerGetFirstAll(); ptPlayerToCheck; ptPlayerToCheck = PlayerGetNextAll(ptPlayerToCheck))
    {
        if (sgptPartyTabGlobals->GetPlayer() != ptPlayerToCheck)
        {
            if (nIndex == m_nSelectedIndex)
            {
                UIText & tTxtControl = UI_GetTextFromID(UITXT_PARTY_CONSOLE_CHARSELECT_NAME);
                tTxtControl.SetText(PlayerGetHeroDisplayName(*ptPlayerToCheck, FALSE), FALSE);
            }

            UIID idWnd = UIIDCreateFromParent(UIWND_PARTY_CONSOLE_CHARSELECT, CRefString().Format(".PlayerSlot%d", nIndex));
            UIID idModel = UIIDCreateFromParent(idWnd, ".Model.Hero3DModel");
            RActorID idRActor = UI_GetRActorFromID(idModel).GetRActor();
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor && nIndex == m_nSelectedIndex)
            {
                m_idHighlightEffect = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CHARSELECT), idRActor, RACTORID_INVALID, 0, 0);
            }

            nIndex++;
        }
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void CharSelectScreen::sChangeSelection(const int32 * nDeltas)  // line 498
{
    int32 nPrev = m_nSelectedIndex;
    uint32 nNumPlayers = PlayersGetCount();
    int32 nNew = nDeltas[m_nSelectedIndex] + m_nSelectedIndex;
    if (nNew >= 0 && nNew <= (int32)nNumPlayers - 2)
    {
        m_nSelectedIndex = nNew;
        if (nNew != nPrev)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);
            OnSelectionChanged();
        }
    }
}

void CharSelectScreen::SelectPrevious()  // line 524
{
    static const int32 snDeltas[] = { 1, 0, -2 };
    sChangeSelection(snDeltas);
}

void CharSelectScreen::SelectNext()  // line 534
{
    static const int32 snDeltas[] = { 2, -1, 0 };
    sChangeSelection(snDeltas);
}

void CharSelectScreen::HandleLeftStickInput(const int32 nControllerIndex)  // line 545
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    ::Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput(vecStick, FALSE);
    switch (idRule)
    {
    case ::Console::RULE_STICK_LEFT:
        SelectPrevious();
        break;

    case ::Console::RULE_STICK_RIGHT:
        SelectNext();
        break;
    }
}

static BOOL sCanInspect(const int32 nTargetPlayerIndex)  // line 566
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nTargetPlayerIndex);
    if (ptPlayer && ACDTryToGet(ptPlayer->idPlayerACD))
    {
        return TRUE;
    }
    return FALSE;
}

static TradeResponse sCanTrade(Player * ptPlayerLocal, const int32 nTargetPlayerIndex)  // line 586
{
    if (GameIsSeasonal())
    {
        return TRADE_RESPONSE_ERROR;
    }

    Player * ptPlayerTarget = PlayerGetByPlayerIndex(nTargetPlayerIndex);
    if (ptPlayerTarget && ACDTryToGet(ptPlayerTarget->idPlayerACD))
    {
        return TradeCanTradeWith(*ptPlayerLocal, ptPlayerTarget->idPlayerACD);
    }

    return TRADE_RESPONSE_ERROR;
}

void CharSelectScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)  // line 610
{
    switch (eInputButton)
    {
    case 0:
    {
        int32 nPlayerIndex = m_nPlayerIndexMapping[m_nSelectedIndex];
        if (sCanInspect(nPlayerIndex))
        {
            CPlayerBeginInspect(nPlayerIndex, sgptPartyTabGlobals->GetPlayer());
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            sgptPartyTabGlobals->SetDeferredMode(PartyTab::PTM_INSPECT_INVENTORY);
        }
        break;
    }

    case 1:
        OnCloseButton();
        break;

    case 2:
    {
        int32 nPlayerIndex = m_nPlayerIndexMapping[m_nSelectedIndex];
        if (sCanInspect(nPlayerIndex))
        {
            CPlayerBeginInspect(nPlayerIndex, sgptPartyTabGlobals->GetPlayer());
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            sgptPartyTabGlobals->SetDeferredMode(PartyTab::PTM_INSPECT_SKILLS);
        }
        break;
    }

    case 3:
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(6);
        if (idAction != ACTION_GIVE_GOLD && idAction != ACTION_TRADE)
        {
            return;
        }

        if (GameIsSeasonal())
        {
            return;
        }

        Player * ptPlayerLocal = sgptPartyTabGlobals->GetPlayer();
        int32 nTargetPlayerIndex = m_nPlayerIndexMapping[m_nSelectedIndex];
        TradeResponse eTradeResponse = sCanTrade(ptPlayerLocal, nTargetPlayerIndex);
        if (eTradeResponse == TRADE_RESPONSE_OK)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            Player * ptPlayerTarget = PlayerGetByPlayerIndex(nTargetPlayerIndex);
            if (idAction == ACTION_GIVE_GOLD)
            {
                OnGiveGold(ptPlayerLocal, ptPlayerTarget);
            }
            else if (idAction == ACTION_TRADE)
            {
                OnTradeButton(ptPlayerLocal, ptPlayerTarget);
            }
        }
        else
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            if (eTradeResponse == TRADE_RESPONSE_TOO_FAR)
            {
                tParams.uszMessage = StringListFetch("ConsoleTrade:CannotTradeTooFar");
            }
            else
            {
                tParams.uszMessage = StringListFetch("ConsoleTrade:CannotTrade");
            }
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        }
        break;
    }

    case 14:
    {
        Player * ptPlayerLocal = sgptPartyTabGlobals->GetPlayer();
        int32 nTargetPlayerIndex = m_nPlayerIndexMapping[m_nSelectedIndex];
        Player * ptPlayerTarget = PlayerGetByPlayerIndex(nTargetPlayerIndex);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OnViewGamerCard(ptPlayerLocal, ptPlayerTarget);
        break;
    }
    }
}

void CharSelectScreen::Open()  // line 712
{
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_CHARSELECT).SetActive(TRUE);
    m_nSelectedIndex = 0;

    int32 nIndex = 0;
    for (const Player * ptPlayerToCheck = PlayerGetFirstAll(); ptPlayerToCheck; ptPlayerToCheck = PlayerGetNextAll(ptPlayerToCheck))
    {
        if (sgptPartyTabGlobals->GetPlayer() != ptPlayerToCheck)
        {
            m_nPlayerIndexMapping[nIndex] = ptPlayerToCheck->nPlayerIndex;

            UIID idWnd = UIIDCreateFromParent(UIWND_PARTY_CONSOLE_CHARSELECT, CRefString().Format(".PlayerSlot%d", nIndex));
            UI_GetWindowFromID(idWnd).SetActive(TRUE);

            BOOL fCanInspect = sCanInspect(ptPlayerToCheck->nPlayerIndex);
            UIID idAway = UIIDCreateFromParent(idWnd, ".Away");
            UI_GetWindowFromID(idAway).SetActive(!fCanInspect);
            if (fCanInspect)
            {
                UIID idModel = UIIDCreateFromParent(idWnd, ".Model.Hero3DModel");
                ShowPlayerModel(*ptPlayerToCheck, idModel);
            }

            UIID idTxtPlayerIndicator = UIIDCreateFromParent(idWnd, ".ShortLabel");
            UI_GetTextFromID(idTxtPlayerIndicator).SetText(CPlayerGetIndicatorString(*ptPlayerToCheck), FALSE);

            UIID idPortrait = UIIDCreateFromParent(idWnd, ".Portrait");
            GBID gbidPortraitFrame = VisualInventoryGetCosmeticItemInSlot(ptPlayerToCheck->idPlayerACD, 3);
            UIUtils::Console::DisplayPortrait(ptPlayerToCheck->ePlayerClassPortrait, (ptPlayerToCheck->tHeroStateData.uPlayerFlags >> 1) & 1, ptPlayerToCheck->nCurrentAltLevel, idPortrait, gbidPortraitFrame);

            nIndex++;
        }
    }

    for (; nIndex < 3; nIndex++)
    {
        UIID idWnd = UIIDCreateFromParent(UIWND_PARTY_CONSOLE_CHARSELECT, CRefString().Format(".PlayerSlot%d", nIndex));
        UI_GetWindowFromID(idWnd).SetActive(FALSE);
        m_nPlayerIndexMapping[nIndex] = -1;
    }

    OnSelectionChanged();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void CharSelectScreen::Close()  // line 764
{
    for (int32 nIndex = 0; nIndex < 3; nIndex++)
    {
        UIID idWnd = UIIDCreateFromParent(UIWND_PARTY_CONSOLE_CHARSELECT, CRefString().Format(".PlayerSlot%d", nIndex));
        UIID idModel = UIIDCreateFromParent(idWnd, ".Model.Hero3DModel");
        UI_GetWindowFromID(idWnd).SetActive(FALSE);
        UI_GetRActorFromID(idModel).Cleanup();
    }

    InputClearRemappings();
    CleanupHighlightEffect();

    if (UIStackSplit::Console::IsVisible())
    {
        UIStackSplit::Console::Close();
    }

    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_CHARSELECT).SetActive(FALSE);
}

void CharSelectScreen::sUpdateButtonLegend()  // line 789
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    InputClearRemappings();

    static uint32 sgdwPrevTime = GetMachineTicks();
    uint32 dwNow = GetMachineTicks();
    if (dwNow > sgdwPrevTime + 1000)
    {
        sgdwPrevTime = dwNow;
        tActionManager.SetActionsDirtyFlag(TRUE);
    }

    if (!tActionManager.AreActionsDirty())
    {
        return;
    }

    tActionManager.SetActionsDirtyFlag(FALSE);

    Player * ptPlayerLocal = sgptPartyTabGlobals->GetPlayer();
    Player * ptPlayerTarget = PlayerGetByPlayerIndex(m_nPlayerIndexMapping[m_nSelectedIndex]);

    tActionManager.ClearAllActions();
    tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

    if (ptPlayerTarget)
    {
        if (sCanInspect(m_nPlayerIndexMapping[m_nSelectedIndex]))
        {
            tActionManager.SetAction(4, ACTION_INSPECT, StringListFetch("ConsoleUI:Inspect").str());
            tActionManager.SetAction(5, ACTION_INSPECT_SKILLS, StringListFetch("ConsoleUI:InspectSkills").str());
        }

        if (PlayerIsLocal(ptPlayerTarget) && !GameIsSeasonal())
        {
            ActorCommonData & tACDPlayer = ACDGet(ptPlayerLocal->idPlayerACD);
            if (tACDPlayer.GetCurrencyAmount(0) > 0)
            {
                tActionManager.SetAction(6, ACTION_GIVE_GOLD, StringListFetch("ConsoleUI:GiveGold").str());
            }
        }
        else if (sCanTrade(ptPlayerLocal, m_nPlayerIndexMapping[m_nSelectedIndex]) == TRADE_RESPONSE_OK)
        {
            tActionManager.SetAction(6, ACTION_TRADE, StringListFetch("ConsoleUI:Trade").str());
        }

        ::Console::Online::PartyMember tPartyMember;
        if (::Console::Online::PartyGetPartyMemberForPlayer(ptPlayerTarget, tPartyMember) &&
            UIPlayerOptions::Console::CanShowGamerCard(ptPlayerLocal->nControllerIndex, tPartyMember))
        {
            InputRemapSingleJoyconButton(8, 14);
            tActionManager.SetAction(21, ACTION_VIEW_GAMERCARD, StringListFetch("Console:PlayerOptionViewUserPageNX64").str());
        }
    }

    ::Console::Screens::UpdateButtonLegendText();
}

inline void CharSelectScreen::Update()  // line 864
{
    sUpdateButtonLegend();
}

inline void CharSelectScreen::OnCloseButton()  // line 873
{
    ::Console::Screens::CloseScreenSet(1, 0);
}

void sGiveGold(const ACDID idACDPlayerDst, const uint64 uAmount)  // line 882
{
    if (uAmount)
    {
        GoldTransferMessage tMessage;
        tMessage.annPlayer = ANNFromACD(idACDPlayerDst);
        tMessage.uAmount = uAmount;
        MessageSendToServer(CMSG_TRY_GOLD_TRANSFER, &tMessage, sizeof(tMessage), sgptPartyTabGlobals->GetPlayer());
    }
}

inline void CharSelectScreen::OnGiveGold(Player * ptPlayerLocal, Player * ptPlayerTarget)  // line 898
{
    ActorCommonData & tACDPlayer = ACDGet(ptPlayerLocal->idPlayerACD);
    uint64 uAmount = tACDPlayer.GetCurrencyAmount(0);
    UIStackSplit::Console::Open(ptPlayerTarget->idPlayerACD, 0, uAmount, sGiveGold, UIStackSplit::Console::STACKSPLIT_GOLD_TRADE);
}

inline void CharSelectScreen::OnTradeButton(Player * ptPlayerLocal, Player * ptPlayerTarget)  // line 910
{
    if (UIUtils::Console::PlayerIsInActiveTieredRift(ptPlayerTarget))
    {
        UIGame::DisplayErrorString(StringListFetch("ConsoleErrors:CannotTradeWithPlayersInTieredRift"));
        return;
    }

    ::Console::Screens::CloseScreenSet(1, 0);
    CTradeTryStart(*ptPlayerLocal, *ptPlayerTarget);
}

void CharSelectScreen::OnViewGamerCard(Player * ptPlayerLocal, Player * ptPlayerTarget)  // line 928
{
    ::Console::Online::PartyMember tPartyMember;
    if (::Console::Online::PartyGetPartyMemberForPlayer(ptPlayerTarget, tPartyMember))
    {
        if (UIPlayerOptions::Console::CanShowGamerCard(ptPlayerLocal->nControllerIndex, tPartyMember))
        {
            UIPlayerOptions::Console::ShowGamerCard(ptPlayerLocal->nControllerIndex, tPartyMember.idOnlineUser, tPartyMember.szName, 0x80);
        }
    }
}

InspectInvScreen::InspectInvScreen()  // line 948
    : m_tEquippedView(m_tEquippedItemProvider, UIWND_PARTY_CONSOLE_EQUIPPED, 0)
{
    m_tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_fRingNeedsUpdating = FALSE;
}

void InspectInvScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)  // line 960
{
    m_tEquippedItemProvider.SetFilter(eNewFilterType);

    ResetDetailsScroll();

    if (GetPrimaryView())
    {
        GetPrimaryView()->ForceUpdate(UIInventory::Console::RESET_SELECTION);
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void InspectInvScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)  // line 983
{
    switch (eInputButton)
    {
    case 1:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        sgptPartyTabGlobals->SwitchMode(PartyTab::PTM_CHAR_SELECT);
        break;

    case 6:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            ScrollDetails(-4.0f);
        }
        break;

    case 7:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            ScrollDetails(4.0f);
        }
        break;
    }
}

static void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl)  // line 1022
{
    UIID idRActorControl = UIRACTOR_CONSOLE_RING;
    VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
    if (ptVisualInventory)
    {
        ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, TRUE, TRUE);
        UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", FALSE);
        UIInventory::Console::UpdatePlayerActorUI(idRActorControl, *ptVisualInventory, tPlayer, 0x68700);
    }

    UIUtils::Console::UpdateHeroNameAndLevel(UIWND_PARTY_CONSOLE_INVENTORY_PANE, tPlayer);
    UIID idStatsPanel = UIIDCreateFromParent(UIWND_PARTY_CONSOLE_INVENTORY_PANE, ".CharacterStats");
    UIUtils::Console::UpdateHeroStats(idStatsPanel, tPlayer);
}

BOOL InspectInvScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)  // line 1046
{
    const ACDIDArray & arEquippedItems = m_tEquippedItemProvider.GetItemsWithFilter(eFilterType);
    if (arEquippedItems.size() > 0 && arEquippedItems[0] != ACDID_INVALID)
    {
        return TRUE;
    }
    return FALSE;
}

inline void InspectInvScreen::MarkRingUIAsDirty()  // line 1064
{
    m_fRingNeedsUpdating = TRUE;
}

void InspectInvScreen::Update()  // line 1073
{
    if (m_fRingNeedsUpdating)
    {
        m_fRingNeedsUpdating = FALSE;
        UpdateRingIcons();

        if (GetFilter() == FILTER_NONE)
        {
            SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
        }

        const UIID & idParentControl = UIWND_PARTY_CONSOLE_INVENTORY_PANE;
        UpdatePlayerInfoUI(*GetPlayer(), idParentControl);

        UIID idRActorControl = UIIDCreateFromParent(idParentControl, ".Hero3DModel");
        UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
        AttachLightToModel(tModel);
    }

    UIInventory::Console::UIInventoryScreen::Update();
}

void InspectInvScreen::OnOpen()  // line 1098
{
    Player * ptPlayer = sgptPartyTabGlobals->GetTargetPlayer();

    SetPlayer(ptPlayer);
    m_tEquippedItemProvider.SetPlayer(ptPlayer);

    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    MarkRingUIAsDirty();

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);
}

void InspectInvScreen::OnSetActive(BOOL bActive)  // line 1117
{
    if (bActive)
    {
        const UIID & idParentControl = UIWND_PARTY_CONSOLE_INVENTORY_PANE;
        UpdatePlayerInfoUI(*GetPlayer(), idParentControl);

        PlayerClass eClass = PlayerGetPlayerClass(GetPlayer());
        LPCSTR szLabel;
        switch (eClass)
        {
        case PLAYER_CLASS_DEMONHUNTER:
            szLabel = "UI:Console_CrestDemonHunter";
            break;
        case PLAYER_CLASS_BARBARIAN:
            szLabel = "UI:Console_CrestBarbarian";
            break;
        case PLAYER_CLASS_WIZARD:
            szLabel = "UI:Console_CrestWizard";
            break;
        case PLAYER_CLASS_WITCHDOCTOR:
            szLabel = "UI:Console_CrestWitchDoctor";
            break;
        case PLAYER_CLASS_MONK:
            szLabel = "UI:Console_CrestMonk";
            break;
        case PLAYER_CLASS_CRUSADER:
            szLabel = "UI:Console_CrestCrusader";
            break;
        case PLAYER_CLASS_NECROMANCER:
            szLabel = "UI:Console_CrestCrusader";
            break;
        default:
            szLabel = "";
            break;
        }
        UI_GetWindowFromID(UIWND_PARTY_CONSOLE_INVENTORY_CREST).SetTextureFrame(szLabel);

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIWND_PARTY_CONSOLE_INVENTORY, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIWND_PARTY_CONSOLE_INVENTORY);
    }
}

void InspectInvScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)  // line 1151
{
    tActionManager.ClearAllActions();
    tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(10, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(11, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}

void InspectInvScreen::OnClose()  // line 1166
{
    SimpleMessage tHeader = {};
    MessageSendToServer(CMSG_INSPECT_END, &tHeader, sizeof(tHeader), sgptPartyTabGlobals->GetPlayer());

    ::Console::UIRActorManager::CloseRActor();
    SetFilterType(FILTER_NONE);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
}

inline InspectSkillsScreen::InspectSkillsScreen()  // line 1184
    : m_fCurrentlyOpening(FALSE)
{
    m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
}

static void sGetSkillsListForPlayer(const ActorCommonData & tACDPlayer, SNOArray & listSkills, SNOArray & listTraits)  // line 1194
{
    listSkills.EmptyArray();
    listTraits.EmptyArray();

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer)
    {
        SNO snoSkillKit = tACDPlayer.GetSkillKit();
        ScopedDefinitionLockR<SkillKitDefinition> pdefSkillKit(snoSkillKit);

        {
            int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
            for (int32 i = 0; i < nCount; i++)
            {
                const ActiveSkillEntry & tSkill = pdefSkillKit->arActiveSkillEntries[i];
                if (tSkill.snoPower != -1 && PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower)))
                {
                    if (tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tSkill.snoPower)) &&
                        !tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_FROM_ITEM, tSkill.snoPower)))
                    {
                        listSkills.Add_Tracked(tSkill.snoPower);
                        if (listSkills.GetArraySize() == 6)
                        {
                            break;
                        }
                    }
                }
            }
        }

        {
            int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
            for (int32 i = 0; i < nCount; i++)
            {
                const TraitEntry & tTrait = pdefSkillKit->arTraitEntries[i];
                if (tTrait.snoPower != -1 && PowerIsDisplayed(AutoPowerDefinitionR(tTrait.snoPower)))
                {
                    if (tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tTrait.snoPower)) &&
                        !tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_FROM_ITEM, tTrait.snoPower)))
                    {
                        listTraits.Add_Tracked(tTrait.snoPower);
                        if (listTraits.GetArraySize() == 4)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
}

BOOL sShouldIncludeInRing(const UIID & idControl)  // line 1259
{
    return idControl != UIWND_PARTY_CONSOLE_SKILLS_RING_GLOW &&
           idControl != UIWND_PARTY_CONSOLE_SKILLS_PASSIVE_RING_GLOW;
}

void InspectSkillsScreen::Open()  // line 1269
{
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS).SetActive(TRUE);
    m_fCurrentlyOpening = TRUE;

    Player & tPlayer = *sgptPartyTabGlobals->GetTargetPlayer();

    ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, TRUE, TRUE);
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", FALSE);

    UIID idRActorControl = UIRACTOR_CONSOLE_RING;
    UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
    AttachLightToModel(tModel);

    VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
    if (ptVisualInventory)
    {
        UIInventory::Console::UpdatePlayerActorUI(idRActorControl, *ptVisualInventory, tPlayer, 0x68700);
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer)
    {
        sGetSkillsListForPlayer(*ptACDPlayer, m_listSkills, m_listTraits);
    }

    UpdateSkillRing();

    m_tRingUI.WrapUsingContainerControl(UIWND_PARTY_CONSOLE_SKILLS_RING, UIWND_PARTY_CONSOLE_SKILLS_RING_CURSOR, sShouldIncludeInRing, FALSE);
    UIID idSkill = UISkills::Console::GetUIIDForSkillSlot(0, UIWND_PARTY_CONSOLE_SKILLS_RING);
    m_tRingUI.SelectControl(idSkill, IconSelectionChanged);

    m_fCurrentlyOpening = FALSE;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void InspectSkillsScreen::HandleLeftStickInput(const int32 nControllerIndex)  // line 1310
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    if (m_tRingStickInputFilter.HandleInput(vecStick, FALSE) == ::Console::AnalogStickInputFilter::Rule::CATCH_ALL_INPUT)
    {
        m_tRingUI.UpdateInput(nControllerIndex, IconSelectionChanged, NULL);
    }
}

void InspectSkillsScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)  // line 1324
{
    switch (eInputButton)
    {
    case 1:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        sgptPartyTabGlobals->SwitchMode(PartyTab::PTM_CHAR_SELECT);
        break;

    case 6:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            ScrollDetailInfo(-4.0f);
        }
        break;

    case 7:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            ScrollDetailInfo(4.0f);
        }
        break;
    }
}

inline void InspectSkillsScreen::Close()  // line 1360
{
    SimpleMessage tHeader = {};
    MessageSendToServer(CMSG_INSPECT_END, &tHeader, sizeof(tHeader), sgptPartyTabGlobals->GetPlayer());

    ::Console::UIRActorManager::CloseRActor();
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS).SetActive(FALSE);
}

void InspectSkillsScreen::Update()  // line 1374
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.AreActionsDirty())
    {
        tActionManager.SetActionsDirtyFlag(FALSE);
        tActionManager.ClearAllActions();
        tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());

        if (CanScrollDetails())
        {
            CRefString sScrollPrompt = StringListFetch("ConsoleSkills:ScrollSkillDescriptions");
            tActionManager.SetAction(10, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
            tActionManager.SetAction(11, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
        }

        ::Console::Screens::UpdateButtonLegendText();
    }
}

void InspectSkillsScreen::UpdateSkillSlotIcon(Player * ptPlayer, const int32 nSlot)  // line 1401
{
    UIID idSkillIcon = UISkills::Console::GetUIIDForSkillSlot(nSlot, UIWND_PARTY_CONSOLE_SKILLS_RING);
    BOOL fSlotSelected = GetSelectedSlot() == nSlot;
    SNO snoPower = (nSlot >= 0 && nSlot < m_listSkills.GetArraySize()) ? m_listSkills[nSlot] : -1;
    UIButton & tBtnIcon = UI_GetButtonFromID(idSkillIcon);
    BOOL fEnabled = snoPower != -1;
    tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    tBtnIcon.SetEnabled(fEnabled);
    UISkills::Console::UpdateSkillSlotIcon(idSkillIcon, nSlot, ptPlayer, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
}

void InspectSkillsScreen::UpdateTraitSlotIcon(Player * ptPlayer, const int32 nSlot)  // line 1421
{
    UIID idTraitSlot = UISkills::Console::GetUIIDForTraitSlot(nSlot, UIWND_PARTY_CONSOLE_SKILLS_RING);
    BOOL fSlotSelected = nSlot + 6 == GetSelectedSlot();
    SNO snoPower = (nSlot >= 0 && nSlot < m_listTraits.GetArraySize()) ? m_listTraits[nSlot] : -1;
    UIButton & tBtnIcon = UI_GetButtonFromID(idTraitSlot);
    BOOL fEnabled = snoPower != -1;
    tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    tBtnIcon.SetEnabled(fEnabled);
    UISkills::Console::UpdateTraitSlotIcon(idTraitSlot, nSlot, ptPlayer, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
}

void InspectSkillsScreen::UpdateKanaisPowersSlotIcon(Player * ptPlayer)  // line 1441
{
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON).SetUserData((void *)KANAI_SLOT);

    bool bAssigned = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[0] != -1 ||
                     ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[1] != -1 ||
                     ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[2] != -1;

    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED).SetActive(!bAssigned);
    UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED).SetActive(bAssigned);
}

void InspectSkillsScreen::UpdateSkillRing()  // line 1457
{
    Player * ptPlayer = sgptPartyTabGlobals->GetTargetPlayer();

    for (int32 i = 0; i < 6; i++)
    {
        UpdateSkillSlotIcon(ptPlayer, i);
    }

    for (int32 i = 0; i < 4; i++)
    {
        UpdateTraitSlotIcon(ptPlayer, i);
    }

    UpdateKanaisPowersSlotIcon(ptPlayer);
}

void InspectSkillsScreen::UpdateUIForSelectedSkill(const int32 nIconSlot, Player * ptPlayer)  // line 1477
{
    int32 nSlot = nIconSlot;
    SNO snoPower = (nSlot >= 0 && nSlot < m_listSkills.GetArraySize()) ? m_listSkills[nSlot] : -1;

    CRefString sSkillName = snoPower == -1 ? StringListFetch("ConsoleSkills:Skills") : UISkills::Console::GetSkillName(snoPower);

    UIHotbarButton & tSkillIcon = UI_GetHotbarButtonFromID(UIWND_PARTY_SKILLS_CONSOLE_SKILL_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tSkillIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_SKILL_NAME).SetText(sSkillName.str(), FALSE);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_SKILL_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }
    else
    {
        UISkills::Console::ShowSkillDescription(UITXT_PARTY_SKILLS_CONSOLE_SKILL_INFO, snoPower, tACDPlayer, FALSE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }

    RuneType eRune = PowerGetRuneTypeForACD(tACDPlayer, snoPower);
    UIHotbarButton & tRuneIcon = UI_GetHotbarButtonFromID(UIWND_PARTY_SKILLS_CONSOLE_RUNE_ICON);
    CRefString sRuneTexture = UISkills::Console::GetRuneTextureFrame(eRune, TRUE);
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo(sRuneTexture.str(), tTexFrame);
    tRuneIcon.SetAllTextureFrames(tTexFrame);

    CRefString sRuneName = eRune == -1 ? StringListFetch("ConsoleSkills:SkillRunes") : PowerGetRuneName(snoPower, eRune);
    UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_RUNE_NAME).SetText(sRuneName.str(), FALSE);

    if (eRune == -1)
    {
        UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_RUNE_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }
    else
    {
        UISkills::Console::UpdatePlayerInfo(ptPlayer);
        UISkills::Console::ShowRuneDescription(UITXT_PARTY_SKILLS_CONSOLE_RUNE_INFO, snoPower, eRune, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }
}

BOOL InspectSkillsScreen::CanScrollDetails()  // line 1535
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    if (UISkills::Console::IconIsKanaiPowers(idSelectedIcon))
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY);
    }
    else if (UISkills::Console::IconIsSkill(idSelectedIcon))
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_SKILL_INFO) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_RUNE_INFO);
    }
    else
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO);
    }
}

inline void InspectSkillsScreen::ResetDetailScrollPosition(const UIID & idScrollBox)  // line 1559
{
    UI_GetConsoleScrollBoxFromID(idScrollBox).SetScrollPos(0.0f);
}

inline void InspectSkillsScreen::ResetDetailScrollPositions()  // line 1568
{
    ResetDetailScrollPosition(UISCROLL_PARTY_SKILLS_CONSOLE_SKILL_INFO);
    ResetDetailScrollPosition(UISCROLL_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO);
    ResetDetailScrollPosition(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON);
    ResetDetailScrollPosition(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR);
    ResetDetailScrollPosition(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY);
}

void InspectSkillsScreen::ScrollDetailInfo(const float nDelta)  // line 1581
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    if (UISkills::Console::IconIsKanaiPowers(idSelectedIcon))
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY, nDelta);
    }
    else if (UISkills::Console::IconIsSkill(idSelectedIcon))
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_SKILL_INFO, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_RUNE_INFO, nDelta);
    }
    else
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO, nDelta);
    }
}

void InspectSkillsScreen::UpdateUIForSelectedTrait(const int32 nIconSlot, Player * ptPlayer)  // line 1606
{
    int32 nSlot = nIconSlot - 6;
    SNO snoPower = (nSlot < m_listTraits.GetArraySize() && nSlot >= 0) ? m_listTraits[nSlot] : -1;

    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(UIWND_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    CRefString sPowerName;
    if (!UISkills::GetName(snoPower, sPowerName))
    {
        sPowerName = StringListFetch("ConsoleSkills:PassiveSkillsTitle");
    }
    UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_NAME).SetText(sPowerName.str(), FALSE);

    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned").str(), FALSE);
    }
    else
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        UISkills::Console::ShowSkillDescription(UITXT_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS_INFO, snoPower, tACDPlayer, TRUE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }
}

static void sUpdateKanaisPowerUI(const Player * ptPlayer, LegendaryPowerSlotType eSlot, const UIID & idVisual, const UIID & idNameText, const UIID & idDescText)  // line 1642
{
    CRefString sPowerName;
    CRefString sPowerDesc;
    GBID gbidPower = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[eSlot];
    if (gbidPower == -1)
    {
        sPowerName = StringListFetch("ConsoleSkills:NoKanaiPowerAssigned");
    }
    else
    {
        sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
        sPowerDesc = UIEquipLegendaryPowers::GetPowerTooltipText(gbidPower, false);
    }

    UIEquipLegendaryPowers::ShowPowerIcon(idVisual, gbidPower, TRUE);
    UI_GetTextFromID(idNameText).SetText(sPowerName, FALSE);
    UI_GetTextFromID(idDescText).SetText(sPowerDesc, FALSE);
}

void InspectSkillsScreen::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)  // line 1668
{
    if (idIcon == GetUIIDNone())
    {
        return;
    }

    if (!sCanStillInspectTarget())
    {
        ::Console::Screens::CloseScreenSet(1, 0);
        return;
    }

    Player * ptPlayer = sgptPartyTabGlobals->GetTargetPlayer();

    if (UISkills::Console::IconIsKanaiPowers(idIcon))
    {
        if (bSelected)
        {
            UI_GetWindowFromID(UIWND_PARTY_SKILLS_CONSOLE_KANAIS_POWERS).SetActive(TRUE);
            UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_HEADER).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_NAME).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_DESCRIPTION).SetText(StringListFetch("ConsoleSkills:Cat_Kanai_Description"), FALSE);

            sUpdateKanaisPowerUI(ptPlayer, 0, UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_VISUAL, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_NAME, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_DETAILS);
            sUpdateKanaisPowerUI(ptPlayer, 1, UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_VISUAL, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_NAME, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_DETAILS);
            sUpdateKanaisPowerUI(ptPlayer, 2, UIWND_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_NAME, UITXT_PARTY_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS);
        }
    }
    else
    {
        if (bSelected)
        {
            BOOL fIsSkill = UISkills::Console::IconIsSkill(idIcon);
            int32 nSlot = UISkills::Console::SlotFromIcon(idIcon);
            UI_GetWindowFromID(UIWND_PARTY_SKILLS_CONSOLE_KANAIS_POWERS).SetActive(FALSE);

            SkillCategory eCategory = -1;
            if (fIsSkill)
            {
                SNO snoPower = (nSlot < m_listSkills.GetArraySize() && nSlot >= 0) ? m_listSkills[nSlot] : -1;
                if (snoPower != -1)
                {
                    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                    eCategory = UISkills::Console::GetSkillCategory(snoPower, tACDPlayer, FALSE);
                }
                else
                {
                    eCategory = UISkills::GetSkillCategoryForSlot(nSlot, ptPlayer->idPlayerACD);
                }
            }

            UISkills::Console::ShowCategoryName(UITXT_PARTY_SKILLS_CONSOLE_HEADER, nSlot, eCategory);
            UISkills::Console::ShowCategoryName(UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_NAME, nSlot, eCategory);
            UISkills::Console::ShowCategoryDescription(UITXT_PARTY_SKILLS_CONSOLE_CATEGORY_DESCRIPTION, ptPlayer, nSlot, eCategory);

            UI_GetWindowFromID(UIWND_PARTY_SKILLS_CONSOLE_SKILLS).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_PARTY_SKILLS_CONSOLE_RUNES).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_PARTY_SKILLS_CONSOLE_PASSIVE_SKILLS).SetActive(!fIsSkill);

            UIWindow & tWndCursor = UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_RING_CURSOR);
            if (fIsSkill)
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor");
                UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_RING_GLOW).SetActive(TRUE);
                UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_PASSIVE_RING_GLOW).SetActive(FALSE);
                UpdateUIForSelectedSkill(nSlot, ptPlayer);
            }
            else
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor2");
                UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_RING_GLOW).SetActive(FALSE);
                UI_GetWindowFromID(UIWND_PARTY_CONSOLE_SKILLS_PASSIVE_RING_GLOW).SetActive(TRUE);
                UpdateUIForSelectedTrait(nSlot, ptPlayer);
            }
        }

        UIID idSparkle;
        idSparkle.SetFromParent(idIcon, "LayoutRoot.FX");
        UI_GetWindowFromID(idSparkle).SetActive(bSelected);

        UIID idButton;
        idButton.SetFromParent(idIcon, "LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);

        ResetDetailScrollPositions();
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

PartyTab::PartyTab()  // line 1786
    : m_nControllerIndex(-1)
    , m_ptPlayer(NULL)
    , m_ptTargetPlayer(NULL)
    , m_eMode(PTM_INVALID)
    , m_eModeDeferred(PTM_INVALID)
{
}

void PartyTab::HandleLeftStickInput(const int32 nControllerIndex)  // line 1802
{
    switch (m_eMode)
    {
    case PTM_CHAR_SELECT:
        m_tCharSelectScreen.HandleLeftStickInput(nControllerIndex);
        break;

    case PTM_INSPECT_INVENTORY:
        m_tInspectInvScreen.HandleLeftStickInput(nControllerIndex);
        break;

    case PTM_INSPECT_SKILLS:
        m_tInspectSkillsScreen.HandleLeftStickInput(nControllerIndex);
        break;
    }
}

inline void PartyTab::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)  // line 1824
{
    if (GetPlayer()->nControllerIndex == nControllerIndex)
    {
        switch (m_eMode)
        {
        case PTM_CHAR_SELECT:
            m_tCharSelectScreen.HandleButtonInput(nControllerIndex, eInputButton);
            break;

        case PTM_INSPECT_INVENTORY:
            m_tInspectInvScreen.HandleButtonInput(nControllerIndex, eInputButton);
            break;

        case PTM_INSPECT_SKILLS:
            m_tInspectSkillsScreen.HandleButtonInput(nControllerIndex, eInputButton);
            break;
        }
    }
}

void PartyTab::Update()  // line 1852
{
    switch (m_eMode)
    {
    case PTM_CHAR_SELECT:
        m_tCharSelectScreen.Update();
        break;

    case PTM_INSPECT_INVENTORY:
        if (!sCanStillInspectTarget())
        {
            SwitchMode(PTM_CHAR_SELECT);
            return;
        }
        m_tInspectInvScreen.Update();
        break;

    case PTM_INSPECT_SKILLS:
        if (!sCanStillInspectTarget())
        {
            SwitchMode(PTM_CHAR_SELECT);
            return;
        }
        m_tInspectSkillsScreen.Update();
        break;
    }
}

inline void PartyTab::OnInspectAck(const int32 nPlayerIndex)  // line 1888
{
    if (m_eModeDeferred == PTM_INSPECT_INVENTORY || m_eModeDeferred == PTM_INSPECT_SKILLS)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        if (ptPlayer)
        {
            UISkills::Console::UpdatePlayerInfo(ptPlayer);
            SetTargetPlayer(ptPlayer);
            SwitchMode(m_eModeDeferred);
            SetDeferredMode(PTM_INVALID);
        }
    }
}

inline void PartyTab::OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)  // line 1906
{
    if (m_eMode == PTM_INSPECT_INVENTORY)
    {
        Player * ptPlayer = GetTargetPlayer();
        if (ptPlayer)
        {
            if ((tOldLoc.eActorLocation == 1 && tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD) ||
                tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
            {
                m_tInspectInvScreen.MarkRingUIAsDirty();
            }
        }
    }
}

void PartyTab::OpenForController(int32 nControllerIndex)  // line 1950
{
    if (nControllerIndex == -1 || IsActive())
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        m_ptPlayer = ptPlayer;
        m_nControllerIndex = nControllerIndex;
        SwitchMode(PTM_CHAR_SELECT);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_PARTY_CONSOLE, FALSE, GetUIIDNone());
        UIInventory::Console::UpdateNavBarTabState(ptPlayer, UIDLG_PARTY_CONSOLE, "LayoutRoot.Navigation", 1);
    }
}

void PartyTab::Close()  // line 1982
{
    if (IsActive())
    {
        SwitchMode(PTM_INVALID);
        ::Console::Screens::CloseModalDialog(UIDLG_PARTY_CONSOLE);
    }
}

inline void PartyTab::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)  // line 1999
{
    if (m_eMode == PTM_INSPECT_SKILLS)
    {
        m_tInspectSkillsScreen.OnIconSelectionChanged(idIcon, bSelected);
    }
}

void PartyTab::SwitchMode(PartyTabMode eNewMode)  // line 2016
{
    switch (m_eMode)
    {
    case PTM_CHAR_SELECT:
        m_tCharSelectScreen.Close();
        break;

    case PTM_INSPECT_INVENTORY:
        m_tInspectInvScreen.Close();
        break;

    case PTM_INSPECT_SKILLS:
        m_tInspectSkillsScreen.Close();
        break;
    }

    m_eMode = eNewMode;

    switch (eNewMode)
    {
    case PTM_CHAR_SELECT:
        m_tCharSelectScreen.Open();
        break;

    case PTM_INSPECT_INVENTORY:
        m_tInspectInvScreen.OpenForController(m_nControllerIndex);
        break;

    case PTM_INSPECT_SKILLS:
        m_tInspectSkillsScreen.Open();
        break;
    }
}

void InitializeForGame()  // line 2053
{
    if (!GameIsStartUp())
    {
        sgptPartyTabGlobals = new PartyTab();
    }
}

void DisposeForGame()  // line 2065
{
    if (!GameIsStartUp())
    {
        delete sgptPartyTabGlobals;
        sgptPartyTabGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)  // line 2077
{
    if (sgptPartyTabGlobals)
    {
        sgptPartyTabGlobals->OpenForController(nControllerIndex);
    }
}

void Close()  // line 2089
{
    if (sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->Close();
    }
}

void OnLeftStick(const UIID & id, const int32 nControllerIndex)  // line 2103
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnButton1(const UIID & id, const int32 nControllerIndex)  // line 2116
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 0);
    }
}

void OnButton2(const UIID & id, const int32 nControllerIndex)  // line 2129
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 1);
    }
}

void OnButton3(const UIID & id, const int32 nControllerIndex)  // line 2142
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 2);
    }
}

void OnButton4(const UIID & id, const int32 nControllerIndex)  // line 2155
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 3);
    }
}

void OnButtonL2(const UIID & id, const int32 nControllerIndex)  // line 2168
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 6);
    }
}

void OnButtonR2(const UIID & id, const int32 nControllerIndex)  // line 2180
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 7);
    }
}

void OnButtonL3(const UIID & id, const int32 nControllerIndex)  // line 2192
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, 14);
    }
}

void OnUpdate()  // line 2205
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->Update();
    }
}

void OnInspectAck(const int32 nPlayerIndex)  // line 2217
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->OnInspectAck(nPlayerIndex);
    }
}

void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)  // line 2227
{
    if (sgptPartyTabGlobals && sgptPartyTabGlobals->IsActive())
    {
        sgptPartyTabGlobals->OnInventoryChanged(tOldLoc, tNewLoc, bForce);
    }
}

void ShowPlayerModel(const Player & tPlayer, const UIID & idModel)  // line 2241
{
    UIRActor & tModel = UI_GetRActorFromID(idModel);
    tModel.DeleteRActor();
    AttachLightToModel(tModel);

    VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
    if (ptVisualInventory)
    {
        UIInventory::Console::UpdatePlayerActorUI(idModel, *ptVisualInventory, tPlayer, 0x68700);
    }
}

void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)  // line 2260
{
    sgptPartyTabGlobals->OnIconSelectionChanged(idIcon, bSelected);
}

static BOOL sCanStillInspectTarget()  // line 2270
{
    Player * ptPlayer = sgptPartyTabGlobals->GetTargetPlayer();
    return ACDTryToGet(ptPlayer->idPlayerACD) != NULL;
}

static void AttachLightToModel(UIRActor & tModel)  // line 2284
{
    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tModel.SetLight(idLight);
}

}  // namespace Console
}  // namespace UIParty

void UIInventory::Console::OnPartyButton(const UIID & id, const int32 nControllerIndex, const InputButton eInputButton)  // line 2303
{
    if (UIParty::Console::sgptPartyTabGlobals && UIParty::Console::sgptPartyTabGlobals->IsActive())
    {
        UIParty::Console::sgptPartyTabGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}
