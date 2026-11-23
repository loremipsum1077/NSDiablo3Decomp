// Original file: X/Client/UserInterface/Console/UIConsoleLeaderboardInspect.cpp
// The console leaderboard inspect screens: a ranked hero's equipped items and skills.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ConsoleUIRActorManager.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../../../Common/HotbarButton.h"
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
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Protobuf/Leaderboard.pb.h"
#include "../../../Common/Items.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
// X/Common/Game.cpp
void GameSoftPause(BOOL bPause);
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
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
typedef int32 SkillCategory;
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
void CloseModalDialog(const UIID & idDialog);
void UpdateButtonLegendText();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
void ResetScrollBox(const UIID & idScrollBox);
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
SkillCategory GetSkillCategoryForSlot(int32 nSlot, const PlayerClass ePlayerClass);
BOOL GetName(SNO snoPower, CRefString & sSkillName);
}  // namespace UISkills
// X/Client/UserInterface/Interface/UIEquipLegendaryPowers.cpp
namespace UIEquipLegendaryPowers {
CRefString GetPowerTooltipText(GBID gbidPower, bool bUseBullets);
void ShowPowerIcon(const UIID & idItemUI, GBID gbidPower, PlayerClass ePlayerClass, BOOL bIsMale);
}  // namespace UIEquipLegendaryPowers
// X/Common/Definitions/GameBalance.cpp, X/Common/Powers/Powers.cpp
CRefString GBIDToString(const GBID & gbid);
CRefString PowerGetRuneName(SNO snoPower, const RuneType eRuneType);
// X/Client/UserInterface/UI.cpp, X/Client/Gfx/ImageCollection.cpp
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
// X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UISkills {
namespace Console {
UIID GetUIIDForSkillSlot(const int32 nSlot, const UIID & idParent);
UIID GetUIIDForTraitSlot(const int32 nSlot, const UIID & idRing);
BOOL IconIsKanaiPowers(const UIID & idIcon);
BOOL IconIsSkill(const UIID & idIcon);
int32 SlotFromIcon(const UIID & idIcon);
SkillCategory GetSkillCategory(const SNO snoPower, const PlayerClass ePlayerClass, const BOOL bIsTrait);
void ShowCategoryName(const UIID & id, const int32 nSlot, const SkillCategory eCategory);
void ShowCategoryDescription(const UIID & id, const Player * ptPlayer, const int32 nSlot, const SkillCategory eCategory);
void UpdateSkillSlotIcon(const UIID & idSkillIcon, const int32 nSlot, const Player * ptPlayer, const SNO snoPower, const BOOL fRingEnabled, const BOOL fSlotSelected, const BOOL fOnCooldown, const BOOL fForInspect);
void UpdateTraitSlotIcon(const UIID & idTraitSlot, const int32 nSlot, const Player * ptPlayer, const SNO snoPower, const BOOL fRingEnabled, const BOOL fSlotSelected, const BOOL fOnCooldown, const BOOL fForInspect);
CRefString GetSkillName(const SNO snoPower);
CRefString GetRuneTextureFrame(const RuneType eRuneType, const BOOL bEquipped);
}  // namespace Console
}  // namespace UISkills
// X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp
namespace Console {
namespace Online {
void LeaderboardGetMetadataForUser(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, uint64 idUser, LeaderboardGetScoresCallback pfnCallback);  // X/Client/Console/LobbyService.cpp
void GetChallengeRiftData(int32 nUserIndex, OnlineService::Callback<void (StorageResult, const D3::ChallengeRifts::ChallengeData &, const D3::Leaderboard::WeeklyChallengeData &)> pfnCallback);  // X/Client/Console/LobbyService.cpp
}  // namespace Online
}  // namespace Console
namespace UILeaderboards {
namespace Console {
void NotifyInspectOpenedFromGameSettings(BOOL bOpened);
void ReturnFromInspect();
}  // namespace Console
}  // namespace UILeaderboards
// X/Client/UserInterface/Console/UIConsoleGameSettings.cpp
namespace UIGameSettings {
namespace Console {
void OnReturnFromLeaderboard(int32 nControllerIndex);
}  // namespace Console
}  // namespace UIGameSettings
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
CRefString GetItemDescription(const ACDID idACDItem, const ACDID idACDPlayer, const PlayerClass ePlayerClass, const BOOL bShowStatRanges);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/UI.cpp
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tEquipment, VisualInventoryType eVIT, PlayerClass ePlayerClass, BOOL fFemale, const PRTransform & tTransform, RenderContext eContext, uint32 dwAnimTag, float flScale, BOOL bUnknown);
SNO UI_GetDefaultHeroLight();
// X/Client/UserInterface/Console/UIConsoleCharacterSelect.cpp
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL bIsFemale);
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Common/Definitions/StringList.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_CONSOLE_LEADERBOARDS;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_CREST;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_EQUIPPED;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY_RING;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY_RING_CURSOR;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_CURSOR;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_INFO;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_RUNE_INFO;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_SKILL_INFO;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_DESCRIPTION;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_HEADER;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_DETAILS;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_DETAILS;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_INFO;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_RUNE_INFO;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_RUNE_NAME;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_SKILL_INFO;
extern UIID UITXT_CONSOLE_LEADERBOARDS_SKILLS_SKILL_NAME;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_PASSIVE_RING_GLOW;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_GLOW;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_VISUAL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_VISUAL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_ICON;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_RUNES;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_RUNE_ICON;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_SKILLS;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_SKILL_ICON;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED;
extern UIID UIRACTOR_CONSOLE_LEADERBOARDS_HERO_MODEL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_TITLE;
extern UIID UILIST_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_LIST;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_OTHERS;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_ARM_LEFT;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_ARM_RIGHT;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_BAG;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_BRACERS;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FEET;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FINGER_LEFT;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FINGER_RIGHT;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_HANDS;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_HEAD;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_LEGS;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_NECK;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_SHOULDER;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_TORSO;
extern UIID UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_WAIST;
extern UIID UIRACTOR_CONSOLE_LEADERBOARDS_SKILLS_HERO_MODEL;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR;
extern UIID UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY;

// UIConsoleInventoryView.h lines 703-747; the bodies are not in that header yet.
inline UIInventory::Console::LeaderboardEquippedItem::LeaderboardEquippedItem()
    : m_ePlayerClass(-1)
{
}

inline const ACDIDArray & UIInventory::Console::LeaderboardEquippedItem::GetItemsWithFilter(const UIFilterType eFilterType) const  // line 712
{
    if (eFilterType >= FILTER_PLAYER_END)
    {
        return m_emptyArray;
    }
    return m_arrItems[eFilterType];
}

inline BOOL UIInventory::Console::LeaderboardEquippedItem::CanProvideUnidentifiedItems() const  // line 726
{
    return FALSE;
}

inline UIInventory::Console::UIInventoryViewItemsProvider::ProviderType UIInventory::Console::LeaderboardEquippedItem::GetProviderType() const  // line 741
{
    return ITEM_PROVIDER_NO_PLAYER_ITEMS;
}

inline CRefString UIInventory::Console::LeaderboardEquippedItem::GetItemDescription(const ACDID idACDItem, const BOOL bShowStatRanges) const  // line 747
{
    return UIInventory::Console::GetItemDescription(idACDItem, ACDID_INVALID, m_ePlayerClass, FALSE);
}

namespace UILeaderboardInspect {
namespace Console {

static const int32 KANAI_SLOT = 10;  // line 53
static const int32 MAX_PARTY_SIZE = 3;  // line 55 (DW_AT_const_value 3)

// Defined at line 503 (external, DW_INL_inlined, out-of-line copy removed by the linker);
// SetProviderData (line 110) calls it, so it is declared before the class (inferred).
PlayerClass GetPlayerClass();

class DetailsInvScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x400
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_LIST = 1,
    };
public:
    DetailsInvScreen();
    void Open();

    BOOL IsActive() const  // line 75
    {
        return UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).IsActive();
    }

    void UpdatePartyInfo();

    void OnL2(const UIID &, const int32 nControllerIndex)  // line 84
    {
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            ScrollDetails(-4.0f);
        }
    }

    void OnR2(const UIID &, const int32 nControllerIndex)  // line 94
    {
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            ScrollDetails(4.0f);
        }
    }

    void MarkRingUIAsDirty();
    virtual void Update();
    void SetProviderData(const D3::Leaderboard::HeroSnapshot & tHeroSnapshot)  // line 110
    {
        m_tEquippedItemProvider.SetData(tHeroSnapshot, GetPlayerClass());
    }

    void Reset()  // line 116
    {
        m_tEquippedItemProvider.CleanupACDs();
    }

    void UpdateVisualInventory(VisualEquipment & tVisualInventory);
    void PopulateHeroRactor(const UIID & idActor, const VisualEquipment & tVisualEquipment);
private:
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);
    virtual UIFilterType GetDefaultRingSelection() const;
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);
    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);
    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);
    virtual void OnClose();
    virtual void OnSetActive(BOOL bActive);
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);
    UIInventory::Console::LeaderboardEquippedItem m_tEquippedItemProvider;  // 0x1f0
    UIInventory::Console::UIInventoryDetailsView m_tEquippedView;  // 0x380
    BOOL m_fRingNeedsUpdating;  // 0x3f8
};
static_assert(sizeof(DetailsInvScreen) == 0x400, "DetailsInvScreen");

class DetailsSkillsScreen {  // size 0x98
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_DETAIL_INFO = 1,
    };
public:
    DetailsSkillsScreen()  // line 280
        : m_fCurrentlyOpening(FALSE)
    {
        m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
    }

    BOOL IsActive() const  // line 287
    {
        return UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).IsActive();
    }

    void HandleLeftStickInput(const int32);
    void Update();
    void Open();
    void Close();
    void ScrollDetails(const float flDelta);
    void OnL2(const UIID &, const int32 nControllerIndex);
    void OnR2(const UIID &, const int32 nControllerIndex);
    void OnIconSelectionChanged(const UIID &, const BOOL);
    void UpdateSkillSlotIcon(const int32 nSlot);
    void UpdateTraitSlotIcon(const int32 nSlot);
    void UpdateKanaisPowersSlotIcon();
    void UpdateSkillRing();
    void UpdateUIForSelectedSkill(const int32 nSlot);
    void UpdateUIForSelectedTrait(const int32 nIconSlot);
    void SkillsIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    int32 GetSelectedSlot() const;
    ::Console::RingWrapper m_tRingUI;  // 0x00
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;  // 0x68
    BOOL m_fCurrentlyOpening;  // 0x90
private:
    BOOL CanScrollDetails();
    void ResetDetailScrollPosition(const UIID & idScrollBox);
    void ResetDetailScrollPositions();
    void ScrollDetailInfo(const float);
};
static_assert(sizeof(DetailsSkillsScreen) == 0x98, "DetailsSkillsScreen");

enum class DetailsScreen : int {
    NONE = -1,
    INVENTORY = 0,
    SKILLS = 1,
};

struct UILeaderboardInspectData {  // size 0x660
    UILeaderboardInspectData();
    ~UILeaderboardInspectData();
    void SwitchToScreen(UILeaderboardInspect::Console::DetailsScreen eScreen);
    int32 nControllerIndex;  // 0x00
    ::Console::AnalogStickInputFilter tRingStickInputFilter;  // 0x08
    BOOL bWaitingOnCallback;  // 0x30
    D3::Leaderboard::HeroSnapshot tHeroSnapshot;  // 0x38
    UILeaderboardInspect::Console::DetailsInvScreen tInventoryScreen;  // 0xb0
    UILeaderboardInspect::Console::DetailsSkillsScreen tSkillsScreen;  // 0x4b0
    UILeaderboardInspect::Console::DetailsScreen eCurrentScreen;  // 0x548
    uint64 idCurrentUser;  // 0x550
    BOOL bChallengeRift;  // 0x558
    PlayerClass ePlayerClass;  // 0x55c
    BOOL bFemale;  // 0x560
    int32 nPartyMembers;  // 0x564
    CRefString strPartyMemberNames[3];  // 0x568
    CRefString strPlayerBattleTag;  // 0x5b0
    VisualEquipment m_tVisualEquipment;  // 0x5c8
    ModalNotificationId idLoadingDataDialog;  // 0x658
};
static_assert(sizeof(UILeaderboardInspectData) == 0x660, "UILeaderboardInspectData");

void Close();
void ForceClose();
void sIconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);

// line 415
UILeaderboardInspectData::~UILeaderboardInspectData()
{
}

// line 417
UILeaderboardInspectData::UILeaderboardInspectData()
    : nControllerIndex(-1)
    , bWaitingOnCallback(FALSE)
    , eCurrentScreen(DetailsScreen::NONE)
    , idCurrentUser(0)
    , bChallengeRift(FALSE)
    , bFemale(FALSE)
    , nPartyMembers(0)
    , idLoadingDataDialog(INVALID_MODAL_NOTIFICATION_ID)
{
}

// line 432
// switch at 435 (INVENTORY case 438-440, SKILLS case 446-448), store at 457, switch at 458
// (INVENTORY 461, SKILLS 466), single return at 473
void UILeaderboardInspectData::SwitchToScreen(DetailsScreen eScreen)
{
    switch (eCurrentScreen)
    {
    case DetailsScreen::INVENTORY:
        if (tInventoryScreen.IsActive())
        {
            tInventoryScreen.Close();
        }
        break;
    case DetailsScreen::SKILLS:
        if (tSkillsScreen.IsActive())
        {
            tSkillsScreen.Close();
        }
        break;
    }

    eCurrentScreen = eScreen;

    switch (eScreen)
    {
    case DetailsScreen::INVENTORY:
        tInventoryScreen.Open();
        break;
    case DetailsScreen::SKILLS:
        tSkillsScreen.Open();
        break;
    }
}

// line 501
UILeaderboardInspectData * sgptUILeaderboardInspectData;

// line 503; the load sits on line 506
PlayerClass GetPlayerClass()
{
    return sgptUILeaderboardInspectData->ePlayerClass;
}

// line 509
void InitializeForGame()
{
    sgptUILeaderboardInspectData = new UILeaderboardInspectData();
}

// line 515
void DisposeForGame()
{
    delete sgptUILeaderboardInspectData;
    sgptUILeaderboardInspectData = NULL;
}

// line 779
void sOnGetChallengeRiftData(int32 nControllerIndex, LeaderboardIds leaderboardId, ::Console::Online::StorageResult eResult, const D3::ChallengeRifts::ChallengeData & tChallengeData, const D3::Leaderboard::WeeklyChallengeData & tWeeklyChallengeData);
}  // namespace Console
}  // namespace UILeaderboardInspect
InventorySlotType ActorInventoryTranslateFromSavedSlot(int32 eSavedSlotType);  // X/Common/ActorInventory.cpp:419
namespace UILeaderboardInspect {
namespace Console {

void sLeaderboardMetadataRequestCallback(::Console::Online::LeaderboardResult eResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & atScores)
{
    if (!sgptUILeaderboardInspectData->bWaitingOnCallback)
        return;

    sgptUILeaderboardInspectData->bWaitingOnCallback = FALSE;
    if (eResult == ::Console::Online::LeaderboardResult::SUCCESS && atScores.size() > 0 && atScores[0].has_metadata() &&
        atScores[0].metadata().team_member_size() > 0)
    {
        const D3::Leaderboard::HeroSnapshot & tSnapshot = atScores[0].metadata().team_member(0).hero_snapshot();
        sgptUILeaderboardInspectData->tHeroSnapshot.CopyFrom(tSnapshot);
        sgptUILeaderboardInspectData->bFemale = (atScores[0].metadata().team_member(0).hero_flags() >> 1) & 1;
        sgptUILeaderboardInspectData->strPlayerBattleTag = atScores[0].metadata().team_member(0).battle_tag().c_str();
        sgptUILeaderboardInspectData->tInventoryScreen.SetProviderData(sgptUILeaderboardInspectData->tHeroSnapshot);
        sgptUILeaderboardInspectData->tInventoryScreen.UpdateRingIcons();
        if (sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView())
            sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView()->ForceUpdate(UIInventory::Console::KEEP_INDEX_SELECTED);
        sgptUILeaderboardInspectData->tInventoryScreen.UpdateVisualInventory(sgptUILeaderboardInspectData->m_tVisualEquipment);
        sgptUILeaderboardInspectData->tInventoryScreen.PopulateHeroRactor(UIRACTOR_CONSOLE_LEADERBOARDS_HERO_MODEL, sgptUILeaderboardInspectData->m_tVisualEquipment);
        if (sgptUILeaderboardInspectData->bChallengeRift)
            return;

        sgptUILeaderboardInspectData->nPartyMembers = atScores[0].metadata().team_member_size() - 1;
        for (int32 i = 0; i < MAX_PARTY_SIZE; i++)
        {
            if (i < sgptUILeaderboardInspectData->nPartyMembers)
                sgptUILeaderboardInspectData->strPartyMemberNames[i] = atScores[0].metadata().team_member(i + 1).battle_tag().c_str();
            else
                sgptUILeaderboardInspectData->strPartyMemberNames[i] = "";
        }
        sgptUILeaderboardInspectData->tInventoryScreen.UpdatePartyInfo();
        return;
    }

    Close();
    NotificationCreationParams tParams;
    tParams.nControllerIndex = sgptUILeaderboardInspectData->nControllerIndex;
    if (eResult == ::Console::Online::LeaderboardResult::FAILED_NOT_CONNECTED)
        tParams.uszMessage = StringListFetch("ConsoleErrors:LeaderboardNotConnected");
    else
        tParams.uszMessage = StringListFetch("ConsoleErrors:LeaderboardUnknownError");
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}

// Locals pRune (651), pPassive (660), pCubePower (667), pItem (674), eSlot (678), ptView (704) per DWARF.
void sOnGetChallengeRiftData(int32 nControllerIndex, LeaderboardIds leaderboardId, ::Console::Online::StorageResult eResult, const D3::ChallengeRifts::ChallengeData & tChallengeData, const D3::Leaderboard::WeeklyChallengeData & tWeeklyChallengeData)
{
    if (!sgptUILeaderboardInspectData->bWaitingOnCallback)
        return;

    sgptUILeaderboardInspectData->bWaitingOnCallback = FALSE;
    if (sgptUILeaderboardInspectData->idLoadingDataDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgptUILeaderboardInspectData->idLoadingDataDialog);
        sgptUILeaderboardInspectData->idLoadingDataDialog = INVALID_MODAL_NOTIFICATION_ID;
    }

    if (eResult != ::Console::Online::STORAGE_SUCCESS)
        return;

    GameSoftPause(TRUE);
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_LEADERBOARDS, TRUE, GetUIIDNone());
    sgptUILeaderboardInspectData->tInventoryScreen.Reset();
    sgptUILeaderboardInspectData->ePlayerClass = PlayerClassFromGBID(tWeeklyChallengeData.hero_snapshot().digest().gbid_class());
    sgptUILeaderboardInspectData->bFemale = (tWeeklyChallengeData.hero_snapshot().digest().player_flags() >> 1) & 1;
    sgptUILeaderboardInspectData->strPlayerBattleTag = "";
    sgptUILeaderboardInspectData->tHeroSnapshot.Clear();

    for (int32 n = 0; n < tWeeklyChallengeData.hero_snapshot().saved_data().active_skills_size(); n++)
    {
        D3::Profile::SkillWithRune * pRune = sgptUILeaderboardInspectData->tHeroSnapshot.mutable_active_skills()->mutable_runes()->Add();
        pRune->set_skill(tWeeklyChallengeData.hero_snapshot().saved_data().active_skills(n).sno_skill());
        pRune->set_rune_type(tWeeklyChallengeData.hero_snapshot().saved_data().active_skills(n).rune_type());
    }

    for (int32 n = 0; n < tWeeklyChallengeData.hero_snapshot().saved_data().sno_traits_size(); n++)
    {
        int32 * pPassive = sgptUILeaderboardInspectData->tHeroSnapshot.mutable_active_traits()->mutable_sno_traits()->Add();
        *pPassive = tWeeklyChallengeData.hero_snapshot().saved_data().sno_traits(n);
    }

    for (int32 n = 0; n < tWeeklyChallengeData.hero_snapshot().saved_data().gbid_legendary_powers_size(); n++)
    {
        GBID * pCubePower = sgptUILeaderboardInspectData->tHeroSnapshot.mutable_gbid_legendary_powers()->Add();
        *pCubePower = tWeeklyChallengeData.hero_snapshot().saved_data().gbid_legendary_powers(n);
    }

    for (int32 n = 0; n < tWeeklyChallengeData.hero_snapshot().items().items_size(); n++)
    {
        D3::Leaderboard::HeroEquippedItem * pItem = sgptUILeaderboardInspectData->tHeroSnapshot.mutable_equipped_items()->Add();
        pItem->mutable_generator()->CopyFrom(tWeeklyChallengeData.hero_snapshot().items().items(n).generator());
        VisualInventorySlot eSlot = VisualInventorySlotFromInventorySlot(ActorInventoryTranslateFromSavedSlot(tWeeklyChallengeData.hero_snapshot().items().items(n).item_slot()));
        if (eSlot != VIS_INVALID)
            pItem->set_visual_inventory_slot(eSlot);
        pItem->set_item_visual_gbid(pItem->generator().gb_handle().gbid());
        pItem->set_item_effect_type(0);
        pItem->set_effect_level(-1);
        // line 688: set_dye_type is inlined and its has-bit (0x40) is set, but the original stores no
        // value, so the argument is the field's own value (inferred; a load/store pair the optimiser drops)
        pItem->set_dye_type(pItem->dye_type());
    }

    sgptUILeaderboardInspectData->tInventoryScreen.SetProviderData(sgptUILeaderboardInspectData->tHeroSnapshot);
    sgptUILeaderboardInspectData->tInventoryScreen.UpdateRingIcons();
    if (sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView())
        sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView()->ForceUpdate(UIInventory::Console::KEEP_INDEX_SELECTED);
    sgptUILeaderboardInspectData->tInventoryScreen.UpdateVisualInventory(sgptUILeaderboardInspectData->m_tVisualEquipment);
    sgptUILeaderboardInspectData->tInventoryScreen.PopulateHeroRactor(UIRACTOR_CONSOLE_LEADERBOARDS_HERO_MODEL, sgptUILeaderboardInspectData->m_tVisualEquipment);
    UIInventory::Console::UIInventoryView * ptView = sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView();
    if (ptView)
        ptView->ForceUpdate(UIInventory::Console::RESET_SELECTION);

    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_TITLE).SetActive(FALSE);
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_OTHERS).SetActive(FALSE);
    for (int32 n = 1; n <= MAX_PARTY_SIZE; ++n)
    {
        CRefString strPartyPlayer;
        strPartyPlayer.Format(".PartyPlayer%d", n);
        UIID idPartyMember = UIIDCreateFromParent(UILIST_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_LIST, strPartyPlayer.str());
        UI_GetWindowFromID(idPartyMember).SetActive(FALSE);
    }

    sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::INVENTORY);
}

// guard at line 732, body 737-750, single return at 752; the bind is
// function_binder_5::bind (Callback.h:1640, inlined at line 750)
void OpenForChallengeRift(int32 nControllerIndex)
{
    if (!sgptUILeaderboardInspectData->bWaitingOnCallback)
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = StringListFetch("Console:PleaseWait");
        sgptUILeaderboardInspectData->idLoadingDataDialog = ::Console::Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
        sgptUILeaderboardInspectData->bWaitingOnCallback = TRUE;
        sgptUILeaderboardInspectData->bChallengeRift = TRUE;
        sgptUILeaderboardInspectData->idCurrentUser = (uint64)-1;
        sgptUILeaderboardInspectData->nControllerIndex = nControllerIndex;
        UILeaderboards::Console::NotifyInspectOpenedFromGameSettings(TRUE);
        ::Console::Online::GetChallengeRiftData(nControllerIndex,
            OnlineService::internal::function_binder_5<void, int32, LeaderboardIds, ::Console::Online::StorageResult, const D3::ChallengeRifts::ChallengeData &, const D3::Leaderboard::WeeklyChallengeData &>::bind<&sOnGetChallengeRiftData>(nControllerIndex, LB_CHALLENGE_RIFT_FIRST));
    }
}

void Open(int32 nControllerIndex, const LeaderboardKeyAndPartition & tKey, int64 idUser, PlayerClass ePlayerClass)
{
    if (sgptUILeaderboardInspectData->idCurrentUser != idUser)
    {
        sgptUILeaderboardInspectData->tInventoryScreen.Reset();
        UIInventory::Console::UIInventoryView * ptView = sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView();
        if (ptView)
            ptView->ForceUpdate(UIInventory::Console::RESET_SELECTION);
    }
    sgptUILeaderboardInspectData->idCurrentUser = idUser;
    sgptUILeaderboardInspectData->bWaitingOnCallback = TRUE;
    ::Console::Online::LeaderboardGetMetadataForUser(nControllerIndex, tKey, idUser, OnlineService::internal::function_bind_0_2<void, ::Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > &, &sLeaderboardMetadataRequestCallback>());
    sgptUILeaderboardInspectData->nControllerIndex = nControllerIndex;
    sgptUILeaderboardInspectData->ePlayerClass = ePlayerClass;
    sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::INVENTORY);
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
}

// line 786
void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    Close();
}

// line 807
// switch at line 816, INVENTORY body at 819, SKILLS body at 822, single return at 827
void OnL1(const UIID &, const int32 nControllerIndex)
{
    if (!sgptUILeaderboardInspectData->bWaitingOnCallback)
    {
        switch (sgptUILeaderboardInspectData->eCurrentScreen)
        {
        case DetailsScreen::INVENTORY:
            sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::SKILLS);
            break;
        case DetailsScreen::SKILLS:
            sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::INVENTORY);
            break;
        }
    }
}

// line 829
// switch at line 816, INVENTORY body at 819, SKILLS body at 822, single return at 827
void OnR1(const UIID &, const int32 nControllerIndex)
{
    if (!sgptUILeaderboardInspectData->bWaitingOnCallback)
    {
        switch (sgptUILeaderboardInspectData->eCurrentScreen)
        {
        case DetailsScreen::INVENTORY:
            sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::SKILLS);
            break;
        case DetailsScreen::SKILLS:
            sgptUILeaderboardInspectData->SwitchToScreen(DetailsScreen::INVENTORY);
            break;
        }
    }
}

// line 872
void OnL2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptUILeaderboardInspectData->tInventoryScreen.IsActive())
    {
        sgptUILeaderboardInspectData->tInventoryScreen.OnL2(id, nControllerIndex);
    }
    else
    {
        sgptUILeaderboardInspectData->tSkillsScreen.OnL2(id, nControllerIndex);
    }
}

// line 886
void OnR2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptUILeaderboardInspectData->tInventoryScreen.IsActive())
    {
        sgptUILeaderboardInspectData->tInventoryScreen.OnR2(id, nControllerIndex);
    }
    else
    {
        sgptUILeaderboardInspectData->tSkillsScreen.OnR2(id, nControllerIndex);
    }
}

// line 900
void ForceClose()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).SetActive(FALSE);
    sgptUILeaderboardInspectData->tInventoryScreen.Reset();
    sgptUILeaderboardInspectData->idCurrentUser = (uint64)-1;
    sgptUILeaderboardInspectData->tHeroSnapshot.Clear();
}

// line 911
void Close()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).SetActive(FALSE);
    sgptUILeaderboardInspectData->bWaitingOnCallback = FALSE;
    sgptUILeaderboardInspectData->tInventoryScreen.Reset();

    UIRActor & tRactor = UI_GetRActorFromID(UIRACTOR_CONSOLE_LEADERBOARDS_HERO_MODEL);
    tRactor.Cleanup();
    tRactor.DeleteRActor();

    UIRActor & tSkillsRactor = UI_GetRActorFromID(UIRACTOR_CONSOLE_LEADERBOARDS_SKILLS_HERO_MODEL);
    tSkillsRactor.Cleanup();
    tSkillsRactor.DeleteRActor();

    if (sgptUILeaderboardInspectData->bChallengeRift)
    {
        if (UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS).IsActive())
        {
            GameSoftPause(FALSE);
            ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_LEADERBOARDS);
            ForceClose();
        }

        UILeaderboards::Console::NotifyInspectOpenedFromGameSettings(FALSE);
        sgptUILeaderboardInspectData->bChallengeRift = FALSE;
        UIGameSettings::Console::OnReturnFromLeaderboard(sgptUILeaderboardInspectData->nControllerIndex);
    }
    else
    {
        UILeaderboards::Console::ReturnFromInspect();
    }
}

// line 951
BOOL InspectActive()
{
    return sgptUILeaderboardInspectData->tInventoryScreen.IsActive() || sgptUILeaderboardInspectData->tSkillsScreen.IsActive();
}

// line 957
void OnUpdate(const UIID & id)
{
    if (sgptUILeaderboardInspectData->tInventoryScreen.IsActive())
    {
        sgptUILeaderboardInspectData->tInventoryScreen.Update();
    }
    else if (sgptUILeaderboardInspectData->tSkillsScreen.IsActive())
    {
        sgptUILeaderboardInspectData->tSkillsScreen.Update();
    }
}

// line 983
inline DetailsInvScreen::DetailsInvScreen()
    : m_tEquippedView(m_tEquippedItemProvider, UIWND_CONSOLE_LEADERBOARDS_INSPECT_EQUIPPED, 0)
{
    m_tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_fRingNeedsUpdating = FALSE;
}

// line 995
void DetailsInvScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    m_tEquippedItemProvider.SetFilter(eNewFilterType);

    ResetDetailsScroll();

    if (GetPrimaryView() != NULL)
    {
        GetPrimaryView()->ForceUpdate(0);
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

// line 1058
BOOL DetailsInvScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    const ACDIDArray & arEquippedItems = m_tEquippedItemProvider.GetItemsWithFilter(eFilterType);
    if (arEquippedItems.size() > 0 && arEquippedItems[0] != ACDID_INVALID)
    {
        return TRUE;
    }
    return FALSE;
}

// line 1076
inline void DetailsInvScreen::MarkRingUIAsDirty()
{
    m_fRingNeedsUpdating = TRUE;
}

// line 1084
void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{
    UIInventory::Console::UIInventoryView * ptView = sgptUILeaderboardInspectData->tInventoryScreen.GetPrimaryView();
    ptView->ForceUpdate(2);
    sgptUILeaderboardInspectData->tInventoryScreen.OnRingSelectionChanged(idIcon, bSelected);
    if (bSelected)
    {
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

// line 1100
void DetailsInvScreen::Update()
{
    if (RingHasFocus())
    {
        GetRingUI().UpdateInput(sgptUILeaderboardInspectData->nControllerIndex, IconSelectionChanged);
    }

    if (m_fRingNeedsUpdating)
    {
        m_fRingNeedsUpdating = FALSE;
        UpdateRingIcons();

        if (GetFilter() == FILTER_NONE)
        {
            SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
        }
    }

    UIInventory::Console::UIInventoryScreen::Update();

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(10, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(11, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}

// line 1171
void DetailsInvScreen::Open()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY).SetActive(TRUE);

    ResetRing();
    SetFilterType(GetDefaultRingSelection());
    EnableRing(TRUE);
    MarkRingUIAsDirty();

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);
    SetActive(TRUE);
}

// line 1196
void DetailsInvScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        LPCSTR szLabel;
        switch (sgptUILeaderboardInspectData->ePlayerClass)
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
        default:
            szLabel = "";
            break;
        }
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_CREST).SetTextureFrame(szLabel);

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY).SetActive(TRUE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY).SetActive(FALSE);
    }
}

// line 1231
void DetailsInvScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
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

// line 1246
void DetailsInvScreen::OnClose()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).SetActive(FALSE);
    ::Console::UIRActorManager::CloseRActor();
    SetFilterType(FILTER_NONE);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
}

// line 302
inline void DetailsSkillsScreen::Close()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).SetActive(FALSE);
    UIUtils::Console::ResetScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON);
    UIUtils::Console::ResetScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR);
    UIUtils::Console::ResetScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY);
}

// line 313
inline void DetailsSkillsScreen::ScrollDetails(const float flDelta)
{
    UIUtils::Console::ScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON, flDelta);
    UIUtils::Console::ScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR, flDelta);
    UIUtils::Console::ScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY, flDelta);
}

// line 321
inline void DetailsSkillsScreen::OnL2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
    {
        ScrollDetails(-4.0f);
    }
}

// line 331
inline void DetailsSkillsScreen::OnR2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
    {
        ScrollDetails(4.0f);
    }
}

// line 367
inline int32 DetailsSkillsScreen::GetSelectedSlot() const
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    if (idSelectedIcon != GetUIIDNone())
    {
        return UISkills::Console::SlotFromIcon(idSelectedIcon);
    }
    return -1;
}

#line 1258
// NON_MATCHING: 692 vs 696; only the line-1286 if differs: the original re-tests snoPower == -1 (cmn/b.ne),
// ours reuses the (snoPower == -1) flag of the line-1275 ternary (cbz). The 1275 `!=` order (cols 37/60/104)
// and the 1295 `nSlot >= size || snoPower == -1 ? -1 : rune` form (cols 16/22/24) now match; tried also
// the snoPower if-form and `snoPower == -1 ||` first: no gain.

void DetailsSkillsScreen::UpdateUIForSelectedSkill(
    const int32 nSlot)
{

    SNO snoPower =

        nSlot < sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes_size()
        ?
        sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes(nSlot).skill()
        :
        -1;

    CRefString sSkillName = snoPower != -1 ? UISkills::Console::GetSkillName(snoPower) : StringListFetch("ConsoleSkills:Skills");


    UIHotbarButton & tSkillIcon = UI_GetHotbarButtonFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_SKILL_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tSkillIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_SKILL_NAME).SetText(sSkillName.str(), FALSE);


    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_SKILL_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }




    RuneType eRune =
        nSlot >= sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes_size() || snoPower == -1
        ?
        -1
        :
        sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes(nSlot).rune_type();

    UIHotbarButton & tRuneIcon = UI_GetHotbarButtonFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_RUNE_ICON);
    CRefString sRuneTexture = UISkills::Console::GetRuneTextureFrame(eRune, TRUE);
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo(sRuneTexture.str(), tTexFrame);
    tRuneIcon.SetAllTextureFrames(tTexFrame);


    CRefString sRuneName = eRune == -1 ? StringListFetch("ConsoleSkills:SkillRunes") : PowerGetRuneName(snoPower, eRune);
    UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_RUNE_NAME).SetText(sRuneName.str(), FALSE);


    if (eRune == -1)
    {
        UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_RUNE_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }




}
#line 1037

// line 1325
inline BOOL DetailsSkillsScreen::CanScrollDetails()
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    return UISkills::Console::IconIsKanaiPowers(idSelectedIcon) &&
           (UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON) ||
            UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR) ||
            UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY));
}

// line 1342
inline void DetailsSkillsScreen::ResetDetailScrollPosition(const UIID & idScrollBox)
{
    UI_GetConsoleScrollBoxFromID(idScrollBox).SetScrollPos(0.0f);
}

// line 1351
inline void DetailsSkillsScreen::ResetDetailScrollPositions()
{
    ResetDetailScrollPosition(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON);
    ResetDetailScrollPosition(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR);
    ResetDetailScrollPosition(UISCROLL_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY);
}

// line 1362
void DetailsSkillsScreen::UpdateUIForSelectedTrait(const int32 nIconSlot)
{
    int32 nSlot = nIconSlot - 6;
    SNO snoPower = sgptUILeaderboardInspectData->tHeroSnapshot.active_traits().sno_traits(nSlot);

    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    CRefString sPowerName;
    if (!UISkills::GetName(snoPower, sPowerName))
    {
        sPowerName = StringListFetch("ConsoleSkills:PassiveSkillsTitle");
    }
    UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_NAME).SetText(sPowerName.str(), FALSE);

    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned").str(), FALSE);
    }
    UI_GetWindowFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS_INFO).SetActive(snoPower == -1);
}

// line 1394. DWARF: sPowerName/sPowerDesc share line 1400; eClass (1415) and bIsMale (1416) are DWARF locals.
static void sUpdateKanaisPowerUI(int32 eSlot, const UIID & idVisual, const UIID & idNameText, const UIID & idDescText)
{
    CRefString sPowerName, sPowerDesc;
    GBID gbidPower = eSlot < sgptUILeaderboardInspectData->tHeroSnapshot.gbid_legendary_powers_size() ? sgptUILeaderboardInspectData->tHeroSnapshot.gbid_legendary_powers(eSlot) : -1;
    if (gbidPower != -1)
    {
        sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
        sPowerDesc = UIEquipLegendaryPowers::GetPowerTooltipText(gbidPower, false);
    }
    else
    {
        sPowerName = StringListFetch("ConsoleSkills:NoKanaiPowerAssigned");
    }
    PlayerClass eClass = sgptUILeaderboardInspectData->ePlayerClass;
    BOOL bIsMale = !sgptUILeaderboardInspectData->bFemale;
    UIEquipLegendaryPowers::ShowPowerIcon(idVisual, gbidPower, eClass, bIsMale);
    UI_GetTextFromID(idNameText).SetText(sPowerName, FALSE);
    UI_GetTextFromID(idDescText).SetText(sPowerDesc, FALSE);
}

// line 1426
void DetailsSkillsScreen::SkillsIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon == GetUIIDNone())
    {
        return;
    }

    if (UISkills::Console::IconIsKanaiPowers(idIcon))
    {
        if (bSelected)
        {
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS).SetActive(TRUE);
            UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_HEADER).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_NAME).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_DESCRIPTION).SetText(StringListFetch("ConsoleSkills:Cat_Kanai_Description"), FALSE);

            sUpdateKanaisPowerUI(0, UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_VISUAL, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_NAME, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_WEAPON_DETAILS);
            sUpdateKanaisPowerUI(1, UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_VISUAL, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_NAME, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_ARMOR_DETAILS);
            sUpdateKanaisPowerUI(2, UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_NAME, UITXT_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS);

            if (CanScrollDetails())
            {
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                ::Console::Screens::GetActionManager().SetAction(10, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
                ::Console::Screens::GetActionManager().SetAction(11, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
            }
        }
        else
        {
            ResetDetailScrollPositions();
            ::Console::Screens::GetActionManager().ClearAction(10);
            ::Console::Screens::GetActionManager().ClearAction(11);
        }
    }
    else
    {
        if (bSelected)
        {
            BOOL fIsSkill = UISkills::Console::IconIsSkill(idIcon);
            int32 nSlot = UISkills::Console::SlotFromIcon(idIcon);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS).SetActive(FALSE);

            SkillCategory eCategory = -1;
            if (fIsSkill)
            {
                const D3::Leaderboard::HeroSnapshot & tHeroSnapshot = sgptUILeaderboardInspectData->tHeroSnapshot;
                if (nSlot < tHeroSnapshot.active_skills().runes_size())
                {
                    SNO snoPower = tHeroSnapshot.active_skills().runes(nSlot).skill();
                    if (snoPower == -1)
                    {
                        eCategory = UISkills::GetSkillCategoryForSlot(nSlot, sgptUILeaderboardInspectData->ePlayerClass);
                    }
                    else
                    {
                        eCategory = UISkills::Console::GetSkillCategory(snoPower, sgptUILeaderboardInspectData->ePlayerClass, FALSE);
                    }
                }
            }

            UISkills::Console::ShowCategoryName(UITXT_CONSOLE_LEADERBOARDS_SKILLS_HEADER, nSlot, eCategory);
            UISkills::Console::ShowCategoryName(UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_NAME, nSlot, eCategory);
            UISkills::Console::ShowCategoryDescription(UITXT_CONSOLE_LEADERBOARDS_SKILLS_CATEGORY_DESCRIPTION, NULL, nSlot, eCategory);

            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_SKILLS).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_RUNES).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_PASSIVE_SKILLS).SetActive(!fIsSkill);

            UIWindow & tWndCursor = UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_CURSOR);
            if (fIsSkill)
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor");
                UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_GLOW).SetActive(TRUE);
                UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_PASSIVE_RING_GLOW).SetActive(FALSE);
                UpdateUIForSelectedSkill(nSlot);
            }
            else
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor2");
                UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_GLOW).SetActive(FALSE);
                UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_PASSIVE_RING_GLOW).SetActive(TRUE);
                UpdateUIForSelectedTrait(nSlot);
            }
        }

        UIID idSparkle;
        idSparkle.SetFromParent(idIcon, "LayoutRoot.FX");
        UI_GetWindowFromID(idSparkle).SetActive(bSelected);

        UIID idButton;
        idButton.SetFromParent(idIcon, "LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);

        ::Console::Screens::GetActionManager().ClearAction(10);
        ::Console::Screens::GetActionManager().ClearAction(11);
        ResetDetailScrollPositions();
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    ::Console::Screens::UpdateButtonLegendText();
}

// line 1559
void DetailsSkillsScreen::UpdateSkillSlotIcon(const int32 nSlot)
{
    if (nSlot < sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes_size())
    {
        UIID idSkillIcon = UISkills::Console::GetUIIDForSkillSlot(nSlot, UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING);
        SNO snoPower = sgptUILeaderboardInspectData->tHeroSnapshot.active_skills().runes(nSlot).skill();
        UIButton & tBtnIcon = UI_GetButtonFromID(idSkillIcon);
        BOOL fEnabled = snoPower != -1;
        tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        tBtnIcon.SetEnabled(fEnabled);
        BOOL fSlotSelected = GetSelectedSlot() == nSlot;
        UISkills::Console::UpdateSkillSlotIcon(idSkillIcon, nSlot, NULL, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
    }
}

// line 1581
void DetailsSkillsScreen::UpdateTraitSlotIcon(const int32 nSlot)
{
    if (nSlot < sgptUILeaderboardInspectData->tHeroSnapshot.active_traits().sno_traits_size())
    {
        UIID idTraitSlot = UISkills::Console::GetUIIDForTraitSlot(nSlot, UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING);
        SNO snoPower = sgptUILeaderboardInspectData->tHeroSnapshot.active_traits().sno_traits(nSlot);
        UIButton & tBtnIcon = UI_GetButtonFromID(idTraitSlot);
        BOOL fEnabled = snoPower != -1;
        tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        tBtnIcon.SetEnabled(fEnabled);
        BOOL fSlotSelected = nSlot + 6 == GetSelectedSlot();
        UISkills::Console::UpdateTraitSlotIcon(idTraitSlot, nSlot, NULL, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
    }
}

// line 1617
void sIconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)
{
    sgptUILeaderboardInspectData->tSkillsScreen.SkillsIconSelectionChanged(idIcon, bSelected);
}

// line 1628
void DetailsSkillsScreen::UpdateSkillRing()
{
    for (int32 i = 0; i < 6; ++i)
    {
        UpdateSkillSlotIcon(i);
    }

    for (int32 i = 0; i < 4; ++i)
    {
        UpdateTraitSlotIcon(i);
    }

    UpdateKanaisPowersSlotIcon();
}

// line 1604
inline void DetailsSkillsScreen::UpdateKanaisPowersSlotIcon()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON).SetUserData((void *)KANAI_SLOT);
    BOOL bAssigned = sgptUILeaderboardInspectData->tHeroSnapshot.gbid_legendary_powers_size() > 0;
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED).SetActive(!bAssigned);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED).SetActive(bAssigned);
}

// line 1644
void DetailsSkillsScreen::Open()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).SetActive(TRUE);
    UpdateSkillRing();
    sgptUILeaderboardInspectData->tInventoryScreen.PopulateHeroRactor(UIRACTOR_CONSOLE_LEADERBOARDS_SKILLS_HERO_MODEL, sgptUILeaderboardInspectData->m_tVisualEquipment);

    m_tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING, UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING_CURSOR, NULL, FALSE);
    UIID idSkill = UISkills::Console::GetUIIDForSkillSlot(0, UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS_RING);
    m_tRingUI.SelectControl(idSkill, sIconSelectionChanged);
    m_fCurrentlyOpening = FALSE;

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
}

// line 1667
inline void DetailsSkillsScreen::Update()
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(sgptUILeaderboardInspectData->nControllerIndex, FALSE);
    if (m_tRingStickInputFilter.HandleInput(vecStick, FALSE) == 0)
    {
        m_tRingUI.UpdateInput(sgptUILeaderboardInspectData->nControllerIndex, sIconSelectionChanged);
    }
}

// line 203
void DetailsInvScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY_RING, UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY_RING_CURSOR, NULL, FALSE);
}

// line 210
UIFilterType DetailsInvScreen::GetDefaultRingSelection() const
{
    return FILTER_PLAYER_START;
}

// line 239
UIInventory::Console::UIInventoryView * DetailsInvScreen::GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    return &m_tEquippedView;
}

// line 246
UIInventory::Console::UIInventoryView * DetailsInvScreen::GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    return &m_tEquippedView;
}

// line 220
void DetailsInvScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
{
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_ARM_LEFT, FILTER_MAIN_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_ARM_RIGHT, FILTER_OFF_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_BAG, FILTER_BAG);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_BRACERS, FILTER_WRIST);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FEET, FILTER_FEET);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FINGER_LEFT, FILTER_RING_LEFT);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_FINGER_RIGHT, FILTER_RING_RIGHT);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_HANDS, FILTER_HANDS);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_HEAD, FILTER_HEAD);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_LEGS, FILTER_LEGS);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_NECK, FILTER_NECK);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_SHOULDER, FILTER_SHOULDERS);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_TORSO, FILTER_CHEST);
    tFilterMap.SetAt_Tracked(UIBTN_CONSOLE_LEADERBOARDS_INSPECT_RING_WAIST, FILTER_BELT);
}

// line 1017
void DetailsInvScreen::UpdateVisualInventory(VisualEquipment & tVisualInventory)
{
    tVisualInventory.Init();
    for (int32 i = 0; i < NUM_PLAYER_BODYSLOTS; i++)
    {
        const ACDIDArray & arrItems = m_tEquippedItemProvider.m_arrItems[i];
        if (arrItems.size() == 0 || arrItems[0] == ACDID_INVALID)
            continue;

        ActorCommonData * ptACDItem = ACDTryToGet(arrItems[0]);
        if (ptACDItem == NULL)
            return;

        ActorCommonData & tACDItem = *ptACDItem;
        GBID gbidItem = tACDItem.hGB.gbid;
        ScopedItemDefinitionLockR ptItemDef(gbidItem);
        if (ptItemDef == NULL)
            continue;

        ScopedItemTypeDefinitonLockR ptItemTypeDef(ptItemDef->gbidItemType);
        if (ptItemTypeDef == NULL)
            continue;

        GBID gbidTransmog = tACDItem.AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
        DyeType eDyeType = tACDItem.GetDyeType();
        if (i == 0)
        {
            if (ItemIsAllowedInSlot(SLOT_HIRELING_LEFT_HAND, ptItemTypeDef))
            {
                tVisualInventory.tVisualItem[VIS_LEFT_HAND].gbidItem = gbidTransmog != -1 ? gbidTransmog : gbidItem;
                tVisualInventory.tVisualItem[VIS_LEFT_HAND].eDyeType = eDyeType;
            }
        }
        else if (i == 1)
        {
            if (ItemIsAllowedInSlot(SLOT_HIRELING_RIGHT_HAND, ptItemTypeDef))
            {
                tVisualInventory.tVisualItem[VIS_RIGHT_HAND].gbidItem = gbidTransmog != -1 ? gbidTransmog : gbidItem;
                tVisualInventory.tVisualItem[VIS_RIGHT_HAND].eDyeType = eDyeType;
            }
        }
        else
        {
            for (InventorySlotType eSlot = SLOT_PLAYER_START; eSlot < SLOT_PLAYER_BODYSLOT_END; eSlot++)
            {
                if (ItemIsAllowedInSlot(eSlot, ptItemTypeDef))
                {
                    VisualInventorySlot eVisSlot = VisualInventorySlotFromInventorySlot(eSlot);
                    if (eVisSlot != VIS_INVALID)
                    {
                        tVisualInventory.tVisualItem[eVisSlot].gbidItem = gbidTransmog != -1 ? gbidTransmog : gbidItem;
                        tVisualInventory.tVisualItem[eVisSlot].eDyeType = eDyeType;
                    }
                    break;
                }
            }
        }
    }
}

void DetailsInvScreen::PopulateHeroRactor(const UIID & idActor, const VisualEquipment & tVisualEquipment)
{
    PlayerClass eClass = sgptUILeaderboardInspectData->ePlayerClass;
    BOOL bIsFemale = sgptUILeaderboardInspectData->bFemale;
    PRTransform transform(cgqIdentity, cgwpNull);
    SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);

    UIRActor & tRactor = UI_GetRActorFromID(idActor);
    tRactor.Cleanup();
    tRactor.DeleteRActor();

    if (snoActor != SNO_NONE)
    {
        RActorID ractorID = UI_CreateHeroUIRActor(snoActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE), tVisualEquipment, 0, eClass, bIsFemale, transform, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), 0x68700, 1.0f, TRUE);
        tRactor.SetRActor(ractorID);

        LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
        tRactor.SetLight(idLight);
    }
}

// line 1136
// if at line 1151: SetText 1155 and SetActive(TRUE) 1156 in the then-block, SetActive(FALSE) 1160 in the else
void DetailsInvScreen::UpdatePartyInfo()
{
    UI_GetTextFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_TITLE).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:InspectingTitle"), sgptUILeaderboardInspectData->strPlayerBattleTag.str()), FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_TITLE).SetActive(TRUE);

    for (int32 n = 0; n < MAX_PARTY_SIZE; ++n)
    {
        CRefString strPartyPlayer;
        strPartyPlayer.Format(".PartyPlayer%d", n + 1);
        UIID idPartyMember = UIIDCreateFromParent(UILIST_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_LIST, strPartyPlayer.str());
        UIWindow & tWnd = UI_GetWindowFromID(idPartyMember);
        if (n < sgptUILeaderboardInspectData->nPartyMembers)
        {
            UIID idText = UIIDCreateFromParent(idPartyMember, ".Name");
            UI_GetTextFromID(idText).SetText(sgptUILeaderboardInspectData->strPartyMemberNames[n].str(), FALSE);
            tWnd.SetActive(TRUE);
        }
        else
        {
            tWnd.SetActive(FALSE);
        }
    }

    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_INSPECT_OVERLAY_OTHERS).SetActive(sgptUILeaderboardInspectData->nPartyMembers > 0);
}

}  // namespace Console
}  // namespace UILeaderboardInspect
