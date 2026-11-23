#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../../Contrib/Contrib/blz/include/blz/iter_range.h"
#include "../../Console/ControllerUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIEquipLegendaryPowersCommon.h"
#include "../UIID.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
const UIID & UI_GetControllerFocus();
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_KANAIS_COLLECTION;
extern UIID UIWND_KANAIS_COLLECTION_SLOTS;
extern UIID UIWND_KANAIS_COLLECTION_POWERS;
extern UIID UILIST_KANAIS_COLLECTION_LIST_SLOTS;
extern UIID UILIST_KANAIS_COLLECTION_LIST_POWERS;
extern UIID UIWND_KANAIS_COLLECTION_DETAILS_WEAPON;
extern UIID UIWND_KANAIS_COLLECTION_DETAILS_ARMOR;
extern UIID UIWND_KANAIS_COLLECTION_DETAILS_JEWELRY;
extern UIID UISCROLL_KANAIS_COLLECTION_DETAILS;
extern UIID UITXT_KANAIS_COLLECTION_DETAILS_NAME;
extern UIID UITXT_KANAIS_COLLECTION_DETAILS_DESC;
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
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void ScrollContentInScrollBox(const UIID & idScrollBox, float fAmount);
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
BOOL ShowTieredRiftMessage(const Player * ptPlayer);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void UpdateViewItemSelectionUI(const UIID & idItem, const BOOL fShowSelection);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Interface/UITutorials.h:34
#include "../Interface/UITutorials.h"



extern UITutorialManager gtTutorialMgr;
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/CGame.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
void CancelAllChannelingAbilities();
#include "../../../Common/Networking/GameMessage.h"



// AssignLegendaryPowerMessage comes from X/Common/Definitions/HeroDefinition.h through Player.h.
// X/Common/Crafting/ItemCraftingSwig.h
enum {
    CRAFTER_TYPE_HORADRIM = 3,
};
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
// X/Common/Items.cpp
class ActorCommonData;
typedef int32 AttribGroupID;
SNO ItemGetLegendaryPower(GBID gbidItem);
float ItemGetLegendaryPowerValue(GBID gbidItem);
BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem);
BOOL ItemPlayerCanAssignLegendaryPower(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType);
LegendaryPowerSlotType ItemGetLegendaryPowerSlot(GBID gbidItem);
BOOL ItemResolveValueTagForItemPassivePowerDescription(const ActorCommonData *, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, LPCSTR szTag, float & flResultMin, float * pflResultMax, CRefString &, SNO snoPower, int32, void * pData, void *, FontSpecifier &);
// X/Common/UserInterface/UIToolTips.cpp
typedef int32 ToolTipValueColoring;
typedef BOOL (*TagResolutionCallback)(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);
CRefString UIToolTips_ProcessTip(const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const SNO snoPower, const int32 nLevel, LPCSTR szTip, const ToolTipValueColoring eColoringType, TagResolutionCallback pTagCallback, void * pTagData0, void * pTagData1);
// X/Common/UserInterface/UITextControl.cpp
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);
// X/Common/Attributes/FastAttribute.cpp
typedef int32 FastAttribType;
AttribGroupID FastAttribGroupCreate(FastAttribType eType, int32 nDefaultEntryIndex);
void FastAttribGroupDelete(AttribGroupID idAttribGroup);
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue);
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);
// X/Common/Definitions/GameBalanceDefinition.cpp
CRefString GBIDToString(const GBID & gbid);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);

enum ButtonActions {
    ACTION_SELECT = 1,
    ACTION_CLOSE = 2,
    ACTION_BACK = 3,
    ACTION_SCROLL_DETAIL_INFO = 4,
};

// Invented name: the "no assignment pending" value the screen keeps in gbidEquippedPower
// (GBID_NONE, -1, is a valid "slot cleared" assignment).
static const GBID GBID_NO_PENDING_POWER = -2;

class LegendaryPowerSlotDataSource : public ConsoleListBoxDataSource {  // size 0x8
public:
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
};
static_assert(sizeof(LegendaryPowerSlotDataSource) == 0x8, "LegendaryPowerSlotDataSource");

class LegendaryPowerDataSource : public ConsoleListBoxDataSource {  // size 0x58
public:
    LegendaryPowerDataSource();
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    LegendaryPowerSlotType GetSlot() const;
    void SetSlot(LegendaryPowerSlotType eSlot);
    GBID GetPower(const int32 nIndex) const;
    void Refresh();
private:
    LegendaryPowerSlotType m_eSlot;  // 0x08
    GBIDArray m_arrPowers[3];  // 0x10
};
static_assert(sizeof(LegendaryPowerDataSource) == 0x58, "LegendaryPowerDataSource");

enum EquipLegendaryPowersState : int {
    SELECTING_SLOT = 0,
    SELECTING_POWER = 1,
};

struct EquipLegendaryPowersGlobals {  // size 0xc0
    EquipLegendaryPowersGlobals();
    ~EquipLegendaryPowersGlobals();
    int32 nControllerIndex;  // 0x00
    LegendaryPowerSlotDataSource tSlotDataSource;  // 0x08
    LegendaryPowerDataSource tPowerDataSource;  // 0x10
    EquipLegendaryPowersState eState;  // 0x68
    GBID gbidEquippedPower[3];  // 0x6c
    GBIDArray arNewPowers[3];  // 0x78
};
static_assert(sizeof(EquipLegendaryPowersGlobals) == 0xc0, "EquipLegendaryPowersGlobals");

EquipLegendaryPowersGlobals * sgptEqipLegendaryPowersGlobals;

static BOOL sIsActive();
static void sUpdateUIState();
static void sResetListBox(UIID & idListBox);
static void sRebuildPowerLists();
static GBID sGetSelectedPower();
static BOOL sCanScrollPowerDetails();
static void sUpdateControllerActions();
static void sSwitchState(EquipLegendaryPowersState eState);
static void sGoBack();
static void sSelectSlot();
static void sSelectPower();
static void sResetScrollPosition();
static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);

inline EquipLegendaryPowersGlobals::EquipLegendaryPowersGlobals()
    : nControllerIndex(-1), eState(SELECTING_SLOT)
{
    gbidEquippedPower[0] = GBID_NO_PENDING_POWER;
    gbidEquippedPower[1] = GBID_NO_PENDING_POWER;
    gbidEquippedPower[2] = GBID_NO_PENDING_POWER;
}

EquipLegendaryPowersGlobals::~EquipLegendaryPowersGlobals()
{
}

int32 LegendaryPowerSlotDataSource::GetNumItems() const
{
    return 3;
}

UIControlSourceKey LegendaryPowerSlotDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const LPCSTR ITEM_TEMPLATE = "ConsoleInventoryListCubeTemplate:ConsoleInventoryListCubeTemplate_main";
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse(ITEM_TEMPLATE, -1, NULL, NULL);
    return ITEM_SOURCE_KEY;
}

void LegendaryPowerSlotDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

static void sUpdateSlotBackground(const UIID & idParentUI, LegendaryPowerSlotType eSlot, GBID gbidEquippedPower)
{
    UIID idEmptyWeapon = UIIDCreateFromParent(idParentUI, ".LayoutRoot.Cube_EmptyWeapon");
    UIID idEmptyArmor = UIIDCreateFromParent(idParentUI, ".LayoutRoot.Cube_EmptyArmor");
    UIID idEmptyJewelry = UIIDCreateFromParent(idParentUI, ".LayoutRoot.Cube_EmptyJewelry");
    UIID idEquippedPower = UIIDCreateFromParent(idParentUI, ".LayoutRoot.Cube_EquippedPower");

    UI_GetWindowFromID(idEmptyWeapon).SetActive(eSlot == 0 && gbidEquippedPower == GBID_NONE);
    UI_GetWindowFromID(idEmptyArmor).SetActive(eSlot == 1 && gbidEquippedPower == GBID_NONE);
    UI_GetWindowFromID(idEmptyJewelry).SetActive(eSlot == 2 && gbidEquippedPower == GBID_NONE);
    UI_GetWindowFromID(idEquippedPower).SetActive(gbidEquippedPower != GBID_NONE);
}

static CRefString sGetSlotName(LegendaryPowerSlotType eSlot)
{
    if (eSlot < 0 || eSlot >= 3)
        return CRefString("");

    const LPCSTR SLOT_NAMES[] = {
        "SlotNameWeapon",
        "SlotNameArmor",
        "SlotNameJewelry",
    };
    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_KANAIS_CUBE), SLOT_NAMES[eSlot]);
}

static CRefString sGetSlotDescription(LegendaryPowerSlotType eSlot)
{
    if (eSlot < 0 || eSlot >= 3)
        return CRefString("");

    const LPCSTR SLOT_DESCRIPTIONS[] = {
        "Unslotted_WeaponPower",
        "Unslotted_ArmorPower",
        "Unslotted_JewelryPower",
    };
    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_KANAIS_CUBE), SLOT_DESCRIPTIONS[eSlot]);
}

void LegendaryPowerSlotDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    LegendaryPowerSlotType eSlot = nIndex;

    Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
    if (ptPlayer == NULL)
        return;

    GBID gbidPower = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[eSlot];

    sUpdateSlotBackground(idItemUI, eSlot, gbidPower);

    if (gbidPower == GBID_NONE) {
        UIID idPowerName = UIIDCreateFromParent(idItemUI, ".LayoutRoot.PowerName");
        UIText & tPowerNameText = UI_GetTextFromID(idPowerName);
        tPowerNameText.SetText(sGetSlotName(eSlot), FALSE);
        tPowerNameText.SetTextColor(6);

        UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual")).SetActive(FALSE);
    } else {
        UIID idIcon = UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual");
        UI_GetWindowFromID(idIcon).SetActive(TRUE);
        UIEquipLegendaryPowers::ShowPowerIcon(idIcon, gbidPower, TRUE);

        UIID idPowerName = UIIDCreateFromParent(idItemUI, ".LayoutRoot.PowerName");
        CRefString sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
        UIText & tPowerNameText = UI_GetTextFromID(idPowerName);
        tPowerNameText.SetText(sPowerName, FALSE);
        tPowerNameText.SetTextColor(15);
    }

    BOOL bShowStar = sgptEqipLegendaryPowersGlobals->arNewPowers[eSlot].size() != 0;
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemNewElite")).SetActive(bShowStar);

    BOOL bShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIInventory::Console::UpdateViewItemSelectionUI(idItemUI, bShowSelection);
}

inline LegendaryPowerDataSource::LegendaryPowerDataSource()
    : m_eSlot(-1)
{
}

int32 LegendaryPowerDataSource::GetNumItems() const
{
    if (m_eSlot == -1)
        return 0;

    return m_arrPowers[m_eSlot].size();
}

UIControlSourceKey LegendaryPowerDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const LPCSTR ITEM_TEMPLATE = "ConsoleInventoryListCollectionTemplate:ConsoleInventoryListCollectionTemplate_main";
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse(ITEM_TEMPLATE, -1, NULL, NULL);
    return ITEM_SOURCE_KEY;
}

inline LegendaryPowerSlotType LegendaryPowerDataSource::GetSlot() const
{
    return m_eSlot;
}

inline void LegendaryPowerDataSource::SetSlot(LegendaryPowerSlotType eSlot)
{
    m_eSlot = eSlot;
}

inline GBID LegendaryPowerDataSource::GetPower(const int32 nIndex) const
{
    if (m_eSlot == -1)
        return GBID_NONE;

    return m_arrPowers[m_eSlot][nIndex];
}

static void sFilterPowersBySlot(const GBIDArray & arrSource, GBIDArray & arrDest, LegendaryPowerSlotType eSlot)
{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
    if (ptPlayer == NULL)
        return;

    GBIDArray arrUnlocked;
    GBIDArray arrLocked;

    for (GBID gbidPower : arrSource) {
        if (ItemPlayerCanAssignLegendaryPower(*ptPlayer, gbidPower, eSlot)) {
            if (ItemPlayerHasExtractedLegendaryPower(*ptPlayer, gbidPower))
                arrUnlocked.push_back(gbidPower);
            else
                arrLocked.push_back(gbidPower);
        }
    }

    arrDest.insert(arrDest.end(), arrUnlocked.begin(), arrUnlocked.end());
    arrDest.insert(arrDest.end(), arrLocked.begin(), arrLocked.end());
}

inline void LegendaryPowerDataSource::Refresh()
{
    m_arrPowers[0].clear();
    m_arrPowers[1].clear();
    m_arrPowers[2].clear();

    GBIDArray arrAllPowers;
    UIEquipLegendaryPowers::GetAllPowers(arrAllPowers);
    sFilterPowersBySlot(arrAllPowers, m_arrPowers[0], 0);
    sFilterPowersBySlot(arrAllPowers, m_arrPowers[1], 1);
    sFilterPowersBySlot(arrAllPowers, m_arrPowers[2], 2);
}

void LegendaryPowerDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

static BOOL sIsNewPower(LegendaryPowerSlotType eSlot, GBID gbidPower)
{
    if (eSlot < 0 || eSlot >= 3)
        return FALSE;

    GBIDArray & arNewPowers = sgptEqipLegendaryPowersGlobals->arNewPowers[eSlot];
    return blz::find(blz::begin(arNewPowers), blz::end(arNewPowers), gbidPower) != arNewPowers.end();
}

void LegendaryPowerDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
    if (ptPlayer == NULL)
        return;

    GBID gbidPower = sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetPower(nIndex);
    BOOL bUnlocked = ItemPlayerHasExtractedLegendaryPower(*ptPlayer, gbidPower);

    UIID idLockedBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.CubeUnacquiredPower");
    UIID idUnlockedBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.CubeAcquiredPower");
    UI_GetWindowFromID(idLockedBackground).SetActive(!bUnlocked);
    UI_GetWindowFromID(idUnlockedBackground).SetActive(bUnlocked);

    UIID idPowerName = UIIDCreateFromParent(idItemUI, ".LayoutRoot.PowerName");
    CRefString sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
    UIText & tPowerNameText = UI_GetTextFromID(idPowerName);
    tPowerNameText.SetText(sPowerName, FALSE);
    tPowerNameText.SetTextColor(bUnlocked ? 15 : 6);

    UIID idIcon = UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual");
    UIEquipLegendaryPowers::ShowPowerIcon(idIcon, gbidPower, bUnlocked);

    BOOL bShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIInventory::Console::UpdateViewItemSelectionUI(idItemUI, bShowSelection);

    BOOL bShowStar = sIsNewPower(sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetSlot(), gbidPower);
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemNewElite")).SetActive(bShowStar);
}

static BOOL sIsActive()
{
    return UI_GetWindowFromID(UIDLG_KANAIS_COLLECTION).IsActive();
}

void UIEquipLegendaryPowers::InitializeForGame()
{
    sgptEqipLegendaryPowersGlobals = new EquipLegendaryPowersGlobals;
}

void UIEquipLegendaryPowers::DisposeForGame()
{
    delete sgptEqipLegendaryPowersGlobals;
    sgptEqipLegendaryPowersGlobals = NULL;
}

void UIEquipLegendaryPowers::OpenForController(int32 nControllerIndex)
{
    if (sIsActive())
        return;

    CancelAllChannelingAbilities();

    sgptEqipLegendaryPowersGlobals->eState = SELECTING_SLOT;
    sgptEqipLegendaryPowersGlobals->nControllerIndex = nControllerIndex;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer != NULL)
        ptPlayer->eActiveCrafterType = CRAFTER_TYPE_HORADRIM;

    UIConsoleListBox & tSlotListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS);
    tSlotListBox.SetDataSource(sgptEqipLegendaryPowersGlobals->tSlotDataSource);
    tSlotListBox.SetOnSelChangeFunction(sOnListSelectionChanged);
    tSlotListBox.ResetAllItems();

    gtTutorialMgr.ShowTutorial(UILIST_KANAIS_COLLECTION_LIST_SLOTS, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_COLLECTION_SLOTS), NULL, TRUE);

    UIConsoleListBox & tPowerListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_POWERS);
    tPowerListBox.SetDataSource(sgptEqipLegendaryPowersGlobals->tPowerDataSource);
    tPowerListBox.SetOnSelChangeFunction(sOnListSelectionChanged);

    sRebuildPowerLists();

    Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_KANAIS_COLLECTION, FALSE, GetUIIDNone());

    sUpdateUIState();

    UI_SetControllerFocus(UILIST_KANAIS_COLLECTION_LIST_SLOTS);

    sResetScrollPosition();

    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static void sResetUI()
{
    sgptEqipLegendaryPowersGlobals->nControllerIndex = -1;
    UI_SetControllerFocus(GetUIIDNone());

    sgptEqipLegendaryPowersGlobals->gbidEquippedPower[0] = GBID_NO_PENDING_POWER;
    sgptEqipLegendaryPowersGlobals->gbidEquippedPower[1] = GBID_NO_PENDING_POWER;
    sgptEqipLegendaryPowersGlobals->gbidEquippedPower[2] = GBID_NO_PENDING_POWER;

    sgptEqipLegendaryPowersGlobals->arNewPowers[0].clear();
    sgptEqipLegendaryPowersGlobals->arNewPowers[1].clear();
    sgptEqipLegendaryPowersGlobals->arNewPowers[2].clear();
}

void UIEquipLegendaryPowers::Close()
{
    sResetUI();

    UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS).SetOnSelChangeFunction(NULL);
    UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_POWERS).SetOnSelChangeFunction(NULL);

    Console::Screens::CloseModalDialog(UIDLG_KANAIS_COLLECTION);
}

void UIEquipLegendaryPowers::MarkPowerAsNew(GBID gbidPower)
{
    LegendaryPowerSlotType eSlot = ItemGetLegendaryPowerSlot(gbidPower);
    if (eSlot == -1)
        return;

    GBIDArray & arNewPowers = sgptEqipLegendaryPowersGlobals->arNewPowers[eSlot];
    if (blz::find(blz::begin(arNewPowers), blz::end(arNewPowers), gbidPower) != arNewPowers.end())
        return;

    arNewPowers.push_back(gbidPower);
}

static void sUpdateSlot(const Player & tPlayer, LegendaryPowerSlotType eSlot)
{
    if (sgptEqipLegendaryPowersGlobals->gbidEquippedPower[eSlot] == tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[eSlot]) {
        sgptEqipLegendaryPowersGlobals->gbidEquippedPower[eSlot] = GBID_NO_PENDING_POWER;
        UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS).RefreshItem(eSlot);
    }
}

static void sUpdateDetails(const Player & tPlayer)
{
    LegendaryPowerSlotType eSlot = -1;
    GBID gbidSelectedPower = GBID_NONE;

    switch (sgptEqipLegendaryPowersGlobals->eState) {
    case SELECTING_SLOT: {
        int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS).GetSelectedIndex();
        if (nIndex != -1) {
            eSlot = nIndex;
            gbidSelectedPower = tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[eSlot];
        }
        break;
    }
    case SELECTING_POWER: {
        int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_POWERS).GetSelectedIndex();
        if (nIndex != -1) {
            eSlot = sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetSlot();
            gbidSelectedPower = sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetPower(nIndex);
        }
        break;
    }
    }

    CRefString sName;
    CRefString sDesc;

    if (gbidSelectedPower == GBID_NONE) {
        sName = sGetSlotName(eSlot);
        sDesc = sGetSlotDescription(eSlot);
    } else {
        sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidSelectedPower).str());

        SNO snoPower = ItemGetLegendaryPower(gbidSelectedPower);
        if (snoPower != SNO_NONE) {
            CRefString sRawDesc = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS), SNOToString(POWER_SNO, snoPower, FALSE).str());
            Set_Text_Color(sRawDesc, 15);
            float flPowerValue = ItemGetLegendaryPowerValue(gbidSelectedPower);
            AttribGroupID idAttribGroup = FastAttribGroupCreate(2, -1);
            FastAttribSetValueFloat(idAttribGroup, MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower), flPowerValue);
            sDesc = UIToolTips_ProcessTip(NULL, idAttribGroup, -1, snoPower, 1, sRawDesc, 1, ItemResolveValueTagForItemPassivePowerDescription, (void *)ATTRIBUTE_ITEM_POWER_PASSIVE, NULL);
            FastAttribGroupDelete(idAttribGroup);
        }

        if (!ItemPlayerHasExtractedLegendaryPower(tPlayer, gbidSelectedPower)) {
            sDesc += "\n\n";
            sDesc += StringListFetch("Console_Kanais_Cube:NotCollected");
        }
    }

    UI_GetTextFromID(UITXT_KANAIS_COLLECTION_DETAILS_NAME).SetText(sName, FALSE);
    UI_GetTextFromID(UITXT_KANAIS_COLLECTION_DETAILS_DESC).SetText(sDesc, FALSE);

    UI_GetWindowFromID(UIWND_KANAIS_COLLECTION_DETAILS_WEAPON).SetActive(eSlot == 0);
    UI_GetWindowFromID(UIWND_KANAIS_COLLECTION_DETAILS_ARMOR).SetActive(eSlot == 1);
    UI_GetWindowFromID(UIWND_KANAIS_COLLECTION_DETAILS_JEWELRY).SetActive(eSlot == 2);
}

void UIEquipLegendaryPowers::OnUpdate(const UIID & id)
{
    if (!sIsActive())
        return;

    Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
    if (ptPlayer == NULL)
        return;

    sUpdateSlot(*ptPlayer, 0);
    sUpdateSlot(*ptPlayer, 1);
    sUpdateSlot(*ptPlayer, 2);

    sUpdateDetails(*ptPlayer);

    if (Console::Screens::GetActionManager().AreActionsDirty())
        sUpdateControllerActions();
}

void UIEquipLegendaryPowers::OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (nControllerIndex != sgptEqipLegendaryPowersGlobals->nControllerIndex)
        return;

    Console::ControllerActionId idAction = Console::Screens::GetActionManager().GetActionForInput(4);
    if (idAction != ACTION_SELECT)
        return;

    switch (sgptEqipLegendaryPowersGlobals->eState) {
    case SELECTING_SLOT:
        sSelectSlot();
        break;
    case SELECTING_POWER:
        sSelectPower();
        break;
    }
}

void UIEquipLegendaryPowers::OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (nControllerIndex != sgptEqipLegendaryPowersGlobals->nControllerIndex)
        return;

    Console::ControllerActionId idAction = Console::Screens::GetActionManager().GetActionForInput(7);
    switch (idAction) {
    case ACTION_CLOSE:
        Console::Screens::CloseScreenSet(1, 1);
        break;
    case ACTION_BACK:
        sGoBack();
        break;
    }
}

static void sScrollDetails(float flDelta)
{
    UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_COLLECTION_DETAILS, flDelta);
}

void UIEquipLegendaryPowers::OnButtonL2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptEqipLegendaryPowersGlobals == NULL || !sIsActive())
        return;

    if (nControllerIndex != sgptEqipLegendaryPowersGlobals->nControllerIndex)
        return;

    if (Console::Screens::GetActionManager().GetActionForInput(10) == ACTION_SCROLL_DETAIL_INFO) {
        if (!Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
            sScrollDetails(-4.0f);
    }
}

void UIEquipLegendaryPowers::OnButtonR2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptEqipLegendaryPowersGlobals == NULL || !sIsActive())
        return;

    if (nControllerIndex != sgptEqipLegendaryPowersGlobals->nControllerIndex)
        return;

    if (Console::Screens::GetActionManager().GetActionForInput(11) == ACTION_SCROLL_DETAIL_INFO) {
        if (!Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
            sScrollDetails(4.0f);
    }
}

static void sUpdateUIState()
{
    UI_GetWindowFromID(UIWND_KANAIS_COLLECTION_SLOTS).SetActive(sgptEqipLegendaryPowersGlobals->eState == SELECTING_SLOT);
    UI_GetWindowFromID(UIWND_KANAIS_COLLECTION_POWERS).SetActive(sgptEqipLegendaryPowersGlobals->eState == SELECTING_POWER);
}

static void sResetListBox(UIID & idListBox)
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(idListBox);
    tListBox.ResetAllItems();
}

static void sRebuildPowerLists()
{
    sgptEqipLegendaryPowersGlobals->tPowerDataSource.Refresh();

    sResetListBox(UILIST_KANAIS_COLLECTION_LIST_POWERS);
}

static GBID sGetSelectedPower()
{
    if (sgptEqipLegendaryPowersGlobals->eState != SELECTING_POWER)
        return GBID_NONE;

    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_POWERS).GetSelectedIndex();
    if (nSelectedIndex == -1)
        return GBID_NONE;

    return sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetPower(nSelectedIndex);
}

static BOOL sCanScrollPowerDetails()
{
    return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_KANAIS_COLLECTION_DETAILS);
}

static void sUpdateControllerActions()
{
    Console::ControllerActionManager & tActionManager = Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    switch (sgptEqipLegendaryPowersGlobals->eState) {
    case SELECTING_SLOT:
        tActionManager.SetAction(4, ACTION_SELECT, StringListFetch("H2OLayout:Select"));
        tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
        break;
    case SELECTING_POWER: {
        Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
        if (ptPlayer != NULL) {
            GBID gbidPower = sGetSelectedPower();
            if (ItemPlayerHasExtractedLegendaryPower(*ptPlayer, gbidPower))
                tActionManager.SetAction(4, ACTION_SELECT, StringListFetch("H2OLayout:Select"));
        }
        tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back"));
        break;
    }
    }

    if (sCanScrollPowerDetails()) {
        CRefString sScrollPrompt = StringListFetch("Console_Kanais_Cube:Button_ScrollPowerDescription");
        tActionManager.SetAction(10, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
        tActionManager.SetAction(11, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
    }

    Console::Screens::UpdateButtonLegendText();
    Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
}

static void sSwitchState(EquipLegendaryPowersState eState)
{
    if (sgptEqipLegendaryPowersGlobals->eState == eState)
        return;

    sgptEqipLegendaryPowersGlobals->eState = eState;

    switch (eState) {
    case SELECTING_SLOT:
        UI_SetControllerFocus(UILIST_KANAIS_COLLECTION_LIST_SLOTS);
        for (int32 nPowerSlot = 0; nPowerSlot < 3; nPowerSlot++) {
            if (sgptEqipLegendaryPowersGlobals->gbidEquippedPower[nPowerSlot] != GBID_NO_PENDING_POWER) {
                gtTutorialMgr.ShowTutorial(UIWND_KANAIS_COLLECTION_SLOTS, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_EQUIP_FIRST_POWER), NULL, TRUE);
                break;
            }
        }
        break;
    case SELECTING_POWER: {
        UI_SetControllerFocus(UILIST_KANAIS_COLLECTION_LIST_POWERS);
        GBID gbidPower = sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetPower(0);
        if (gbidPower != GBID_NONE && ItemPlayerHasExtractedLegendaryPower(*PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex), gbidPower))
            gtTutorialMgr.ShowTutorial(UIWND_KANAIS_COLLECTION_POWERS, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_NEWPOWER), NULL, TRUE);
        break;
    }
    }

    sUpdateUIState();
    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static void sGoBack()
{
    if (sgptEqipLegendaryPowersGlobals->eState != SELECTING_POWER)
        return;

    sgptEqipLegendaryPowersGlobals->arNewPowers[sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetSlot()].clear();

    sSwitchState(SELECTING_SLOT);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
}

static void sSelectSlot()
{
    int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS).GetSelectedIndex();
    if (nIndex == -1)
        return;

    sgptEqipLegendaryPowersGlobals->tPowerDataSource.SetSlot(nIndex);
    sResetListBox(UILIST_KANAIS_COLLECTION_LIST_POWERS);

    sSwitchState(SELECTING_POWER);
}

static void sSelectPower()
{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptEqipLegendaryPowersGlobals->nControllerIndex);
    if (ptPlayer == NULL)
        return;

    if (UIUtils::Console::ShowTieredRiftMessage(ptPlayer))
        return;

    GBID gbidPower = sGetSelectedPower();
    if (gbidPower == GBID_NONE)
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_INVENTORY_OPEN), cgtPSODefaults);

    int32 nSlot = sgptEqipLegendaryPowersGlobals->tPowerDataSource.GetSlot();

    AssignLegendaryPowerMessage tMessage;
    tMessage.gbidLegendaryItem = gbidPower;
    tMessage.nSlot = nSlot;
    MessageSendToServer(CMSG_TRY_ASSIGN_LEGENDARY_POWER, &tMessage, sizeof(tMessage), ptPlayer);

    sgptEqipLegendaryPowersGlobals->gbidEquippedPower[nSlot] = gbidPower;

    UI_GetConsoleListBoxFromID(UILIST_KANAIS_COLLECTION_LIST_SLOTS).SetSelectedIndex(nSlot);
    sSwitchState(SELECTING_SLOT);
}

static void sResetScrollPosition()
{
    UI_GetConsoleScrollBoxFromID(UISCROLL_KANAIS_COLLECTION_DETAILS).SetScrollPos(0.0f);
}

static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    sResetScrollPosition();
    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
