#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "ConsoleInputUtilities.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIBlinkerControl.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UILoreCommon.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/Definitions/LoreDefinition.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Quests/QuestsSwig.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/DataStructures/XGrowableArray.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

#include <stdlib.h>
#include <string.h>

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../../Common/Quests/Quests.h"

















typedef void (*QuestIterateCallback)(QuestInstance &, void *);
// X/Common/Quests/Quests.cpp
void QuestsIterate(QuestIterateCallback pfnCallback, void * pUserData);
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACD);
QuestInstance * GetLeadingMainLineQuest();
SNO QuestGetActiveBountyForLevelArea(SNO snoLevelArea, BOOL bUnused);
CRefString QuestStepGetDescription(SNO snoQuest, int32 nStepUID, PlayerClass ePlayerClass);
// X/Common/Definitions/LevelAreaDefinition.cpp
int32 LevelAreaGetDungeonFinderLevel(SNO snoLevelArea);
// X/Common/ActorCommonData.cpp
SNO ACDGetLevelArea(const ActorCommonData & tACD);
// X/Common/Game.cpp
BOOL GameIsOpenWorld();
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
PlayerClass PlayerGetPlayerClass(ACDID idPlayer);
// X/Common/Conversation.cpp
CRefString Conversation_GetFirstLine(SNO snoConversation, const Player * ptPlayer);
// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel);
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIBlinker & UI_GetBlinkerFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_LORE_CONSOLE;
extern UIID UIWND_LORE_CONSOLE_RING;
extern UIID UIWND_LORE_CONSOLE_RING_CURSOR;
extern UIID UIWND_LORE_CONSOLE_RING_GLOW;
extern UIID UIWND_LORE_CONSOLE_PANEL_FOCUS_HIGHLIGHT;
extern UIID UIWND_LORE_CONSOLE_QUEST_PANEL;
extern UIID UILIST_LORE_CONSOLE;
extern UIID UISCROLL_LORE_CONSOLE_SUMMARY;
extern UIID UISCROLL_LORE_CONSOLE_OBJECTIVE_SUMMARY;
extern UIID UITXT_LORE_CONSOLE_SUMMARY;
extern UIID UITXT_LORE_CONSOLE_TITLE;
extern UIID UITXT_LORE_CONSOLE_TYPE;
extern UIID UITXT_LORE_CONSOLE_TYPE_DESCRIPTION;
extern UIID UITXT_LORE_CONSOLE_OBJECTIVE;
extern UIID UITXT_LORE_CONSOLE_OBJECTIVE_HEADER;
extern UIID UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY;
extern UIID UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY_HEADER;
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
void ResetScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UIQuests.cpp
namespace UIQuests {
CRefString GetCurrentObjectiveText(QuestInstance * ptQuestInst);
}  // namespace UIQuests
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
namespace UIInventory {
namespace Console {
void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idParent, LPCSTR szNavBar, int32 nTab);
}  // namespace Console
}  // namespace UIInventory
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
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
}  // namespace Screens
}  // namespace Console

namespace UILore {
namespace Console {

static const int32 ObjectiveFontBoostSize = 2;

enum Slot {
    SLOT_NONE = -1,
    SLOT_BEGIN = 0,
    SLOT_MOST_RECENT = 0,
    SLOT_QUEST = 1,
    SLOT_WORLD = 2,
    SLOT_PEOPLE = 3,
    SLOT_BESTIARY = 4,
    SLOT_OBJECTIVE = 5,
    SLOT_END = 6,
};

enum ButtonActions {
    ACTION_OPEN_SLOT = 1,
    ACTION_CLOSE_SLOT = 2,
    ACTION_CLOSE = 3,
    ACTION_SCROLL = 4,
    ACTION_LISTEN = 5,
};

class NewLoreIndicatorMgr {  // size 0x28
public:
    NewLoreIndicatorMgr();
    void MarkAsNew(const SNO snoLore);
    void UnMarkAsNew(const SNO snoLore);
    void Clear();
    BOOL IsNew(const SNO snoLore) const;
    BOOL CategoryHasNewLore(const LoreCategory eCategory) const;
private:
    blz::vector<int, blz::allocator<int> > m_list;  // 0x00
    int32 m_nCategoryLoreCount[4];  // 0x18
};

inline NewLoreIndicatorMgr::NewLoreIndicatorMgr()
{
    Clear();
}

void NewLoreIndicatorMgr::MarkAsNew(const SNO snoLore)
{
    if (blz::find(m_list.begin(), m_list.end(), snoLore) == m_list.end())
    {
        m_list.push_back(snoLore);
        ScopedLoreDefinitionLockR pdefLore(snoLore);
        m_nCategoryLoreCount[pdefLore->eCategory]++;
    }
}

inline void NewLoreIndicatorMgr::UnMarkAsNew(const SNO snoLore)
{
    const int32 nIndex = blzext::vector_find(m_list, snoLore);
    if (nIndex != -1)
    {
        blzext::vector_erase_at(m_list, nIndex);
        ScopedLoreDefinitionLockR pdefLore(snoLore);
        m_nCategoryLoreCount[pdefLore->eCategory]--;
    }
}

inline void NewLoreIndicatorMgr::Clear()
{
    m_list.clear();
    SigmaMemoryClear(m_nCategoryLoreCount, sizeof(m_nCategoryLoreCount));
}

inline BOOL NewLoreIndicatorMgr::IsNew(const SNO snoLore) const
{
    return blzext::vector_find(m_list, snoLore) != -1;
}

inline BOOL NewLoreIndicatorMgr::CategoryHasNewLore(const LoreCategory eCategory) const
{
    if (eCategory < LORECAT_QUEST || eCategory >= NUM_LORE_CATEGORIES)
        return m_list.size() != 0;

    return m_nCategoryLoreCount[eCategory] > 0;
}

class ListViewDataSource : public ConsoleListBoxDataSource {  // size 0x810
public:
    ListViewDataSource();
    virtual ~ListViewDataSource() {}
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    int32 nNumEntries;  // 0x08
    SNO snoCachedLore[512];  // 0x0c
};

inline Slot SlotFromIcon(const UIID & idIcon);
void sRefreshForSelection();
void Refresh();

class PlayerLoreGlobals {  // size 0x2948
public:
    enum SelectionMode {
        SLOT_SELECTION_MODE = 0,
        LORE_SELECTION_MODE = 1,
    };
public:
    UILore::Console::ListViewDataSource m_tListViewDataSource[5];  // 0x00
    UILore::Console::ListViewDataSource * m_ptCurrentListViewDataSource;  // 0x2850
    const Player * ptPlayer;  // 0x2858
    ::Console::RingWrapper m_tRingUI;  // 0x2860
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;  // 0x28c8
    ::Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x28f0
    UILore::Console::NewLoreIndicatorMgr m_tNewLoreMgr;  // 0x2918
    BOOL m_fCurrentlyOpening;  // 0x2940

    PlayerLoreGlobals()
        : m_ptCurrentListViewDataSource(NULL)
        , ptPlayer(NULL)
        , m_fCurrentlyOpening(FALSE)
        , m_eSelectionMode(SLOT_SELECTION_MODE)
    {
        InitAnalogStickRules();
    }

    void OnOpen(const Player * _ptPlayer);
    void OnUpdate();
    void RefreshList();

    Slot GetSelectedSlot() const
    {
        const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
        if (idSelectedIcon != GetUIIDNone())
            return SlotFromIcon(idSelectedIcon);

        return SLOT_NONE;
    }

    void SetSelectionMode(const SelectionMode eMode);
    void RefreshUIForSelectionMode();
    void OnLeftStick(const UIID & id, const int32 nControllerIndex);
    void Listen();
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);

    BOOL IsActive() const
    {
        return UI_GetWindowFromID(UIDLG_LORE_CONSOLE).IsActive();
    }

    void InitRingUI();
    void EnableRingUI(const BOOL bEnable);

    void InitAnalogStickRules()
    {
        const Blizzard::Time::Microsecond DEFAULT_STICK_DELAY = 200;

        m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);

        m_tStickInputFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_VERTICAL);
        m_tStickInputFilter.SetMinimumInputDelay(DEFAULT_STICK_DELAY);
    }

private:
    void CacheLoreForCategory(const LoreCategory eCategory);
    BOOL ShouldEnableRingIcon(const Slot eSlot);
    void UpdateRingIcons();
    void sUpdateButtonLegend();
    SelectionMode m_eSelectionMode;  // 0x2944
};

UILore::Console::PlayerLoreGlobals * sgptLoreGlobals;

void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);

inline ListViewDataSource::ListViewDataSource()
{
    nNumEntries = 0;
}

int32 ListViewDataSource::GetNumItems() const
{
    return nNumEntries;
}

UIControlSourceKey ListViewDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse("ConsoleJournal:ConsoleJournal_main.LayoutRoot.JournalEntries.JournalListTemplate", SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}

void ListViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

void ListViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idNameText = UIIDCreateFromParent(idItemUI, ".JournalEntryHeadline");
    CRefString sText = StringListFetch(CRefString().Format("Lore:%s", SNOToString(LORE_SNO, snoCachedLore[nIndex], FALSE).str()));
    UIText & tText = UI_GetTextFromID(idNameText);
    tText.SetText(sText, FALSE);

    UIID idSelected = UIIDCreateFromParent(idItemUI, ".JournalListSelected");
    UIWindow & tSelectedWnd = UI_GetWindowFromID(idSelected);
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    tSelectedWnd.SetActive(bSelected);

    UIID idNew = UIIDCreateFromParent(idItemUI, ".ItemNew");
    UIWindow & tNewWnd = UI_GetWindowFromID(idNew);
    tNewWnd.SetActive(sgptLoreGlobals->m_tNewLoreMgr.IsNew(snoCachedLore[nIndex]));
}

void PlayerLoreGlobals::OnOpen(const Player * _ptPlayer)
{
    m_fCurrentlyOpening = TRUE;
    ptPlayer = _ptPlayer;

    for (int32 i = LORECAT_LATEST; i < NUM_LORE_CATEGORIES; i++)
        CacheLoreForCategory((LoreCategory)i);

    InitRingUI();
    SetSelectionMode(SLOT_SELECTION_MODE);
    RefreshUIForSelectionMode();

    m_fCurrentlyOpening = FALSE;
}

inline BOOL PlayerLoreGlobals::ShouldEnableRingIcon(const Slot eSlot)
{
    if (eSlot == SLOT_OBJECTIVE)
        return TRUE;

    {
        const int32 nCount = m_tListViewDataSource[(eSlot >= SLOT_BEGIN && eSlot < SLOT_OBJECTIVE) ? eSlot : SLOT_MOST_RECENT].GetNumItems();
        return nCount > 0;
    }
}

static UIID sUIIDForSlot(const Slot eSlot)
{
    LPCSTR szElement;
    switch (eSlot)
    {
    case SLOT_MOST_RECENT:
        szElement = ".Recent";
        break;
    case SLOT_QUEST:
        szElement = ".Quests";
        break;
    case SLOT_WORLD:
        szElement = ".World";
        break;
    case SLOT_PEOPLE:
        szElement = ".People";
        break;
    case SLOT_BESTIARY:
        szElement = ".Bestiary";
        break;
    default:
        szElement = ".Objective";
        break;
    }

    return UIIDCreateFromParent(UIWND_LORE_CONSOLE_RING, szElement);
}

void PlayerLoreGlobals::UpdateRingIcons()
{
    for (int32 i = SLOT_BEGIN; i < SLOT_END; i++)
    {
        const Slot eSlot = (Slot)i;
        const UIID idIcon = sUIIDForSlot(eSlot);

        const BOOL fEnabled = m_tRingUI.IsEnabled() ? ShouldEnableRingIcon(eSlot) : m_tRingUI.GetSelectedControlID() == idIcon;

        UIButton & tBtnIcon = UI_GetButtonFromID(idIcon);
        tBtnIcon.SetShader(fEnabled ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        tBtnIcon.SetEnabled(fEnabled);

        BOOL fHasNewLore = FALSE;
        if (eSlot != SLOT_OBJECTIVE)
        {
            const LoreCategory eLoreCat = (LoreCategory)(eSlot - 1);
            fHasNewLore = m_tNewLoreMgr.CategoryHasNewLore(eLoreCat);
        }

        UI_GetBlinkerFromID(UIIDCreateFromParent(idIcon, "New")).SetActive(fHasNewLore);
    }
}

void PlayerLoreGlobals::sUpdateButtonLegend()
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!tActionManager.AreActionsDirty())
        return;

    tActionManager.SetActionsDirtyFlag(FALSE);
    tActionManager.ClearAllActions();

    switch (m_eSelectionMode)
    {
    case SLOT_SELECTION_MODE:
        if (GetSelectedSlot() == SLOT_OBJECTIVE)
        {
            if (UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_LORE_CONSOLE_OBJECTIVE_SUMMARY))
            {
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(10, ACTION_SCROLL, sScrollPrompt);
                tActionManager.SetAction(11, ACTION_SCROLL, sScrollPrompt);
            }
        }
        else
        {
            tActionManager.SetAction(4, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        }

        tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        break;

    case LORE_SELECTION_MODE:
        tActionManager.SetAction(4, ACTION_LISTEN, StringListFetch("ConsoleQuest:Listen").str());
        tActionManager.SetAction(7, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

        if (UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_LORE_CONSOLE_SUMMARY))
        {
            CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
            tActionManager.SetAction(10, ACTION_SCROLL, sScrollPrompt);
            tActionManager.SetAction(11, ACTION_SCROLL, sScrollPrompt);
        }
        break;
    }

    ::Console::Screens::UpdateButtonLegendText();
}

inline void PlayerLoreGlobals::OnUpdate()
{
    if (ptPlayer != NULL && m_eSelectionMode == SLOT_SELECTION_MODE)
        m_tRingUI.UpdateInput(ptPlayer->nControllerIndex, IconSelectionChanged, NULL);

    sUpdateButtonLegend();
    UpdateRingIcons();
}

struct LoreSortData {  // size 0x20
    SNO snoLore;  // 0x00
    CRefString sName;  // 0x08
};

static int32 sSortLore(const void * arg0, const void * arg1)
{
    LoreSortData * ptEntry0 = (LoreSortData *)arg0;
    LoreSortData * ptEntry1 = (LoreSortData *)arg1;

    const int32 nNew0 = sgptLoreGlobals->m_tNewLoreMgr.IsNew(ptEntry0->snoLore);
    const int32 nNew1 = sgptLoreGlobals->m_tNewLoreMgr.IsNew(ptEntry1->snoLore);
    if (nNew0 != nNew1)
        return nNew1 - nNew0;

    return strcmp(ptEntry0->sName.str(), ptEntry1->sName.str());
}

void PlayerLoreGlobals::CacheLoreForCategory(const LoreCategory eCategory)
{
    const LearnedLore & tLearnedLore = ptPlayer->tHeroStateData.tSavedData.tLearnedLore;
    LoreSortData tSortData[512];
    ListViewDataSource & tListSource = m_tListViewDataSource[eCategory + 1];

    tListSource.nNumEntries = 0;

    for (int32 i = tLearnedLore.m_nLearnedCount - 1; i >= 0; i--)
    {
        if (eCategory == LORECAT_LATEST)
        {
            tListSource.snoCachedLore[tListSource.nNumEntries] = tLearnedLore.m_snoLoreLearned[i];
            tListSource.nNumEntries++;
        }
        else
        {
            ScopedLoreDefinitionLockR pdefLore(tLearnedLore.m_snoLoreLearned[i]);
            if (pdefLore->eCategory == eCategory)
            {
                LoreSortData & tLoreSortData = tSortData[tListSource.nNumEntries++];
                tLoreSortData.snoLore = tLearnedLore.m_snoLoreLearned[i];
                tLoreSortData.sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LORE), SNOToString(LORE_SNO, tLoreSortData.snoLore, FALSE));
            }
        }
    }

    if (eCategory != LORECAT_LATEST && tListSource.nNumEntries > 0)
    {
        qsort(tSortData, tListSource.nNumEntries, sizeof(LoreSortData), sSortLore);

        for (int32 i = 0; i < tListSource.nNumEntries; i++)
            tListSource.snoCachedLore[i] = tSortData[i].snoLore;
    }
}

void PlayerLoreGlobals::RefreshList()
{
    const Slot eSlot = GetSelectedSlot();

    if (ptPlayer != NULL && UI_GetWindowFromID(UIDLG_LORE_CONSOLE).IsActive() && eSlot != SLOT_NONE && eSlot != SLOT_OBJECTIVE)
    {
        const LoreCategory eCategory = (LoreCategory)(eSlot - 1);
        m_ptCurrentListViewDataSource = &m_tListViewDataSource[(eSlot >= SLOT_BEGIN && eSlot < SLOT_OBJECTIVE) ? eCategory + 1 : 0];

        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_LORE_CONSOLE);
        tListBox.SetDataSource(*m_ptCurrentListViewDataSource);
        tListBox.ResetAllItems();
        if (tListBox.GetNumItems() != 0)
            tListBox.SetSelectedIndex(0);
        else
            sRefreshForSelection();

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

static BOOL sShouldIncludeInRing(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}

void PlayerLoreGlobals::InitRingUI()
{
    m_tRingUI.WrapUsingContainerControl(UIWND_LORE_CONSOLE_RING, UIWND_LORE_CONSOLE_RING_CURSOR, sShouldIncludeInRing, FALSE);

    for (int32 i = SLOT_BEGIN; i < SLOT_END; i++)
    {
        UIID id = sUIIDForSlot((Slot)i);
        UI_GetWindowFromID(id).SetUserData((void *)(intptr_t)i);
    }
}

inline void PlayerLoreGlobals::EnableRingUI(const BOOL bEnable)
{
    m_tRingUI.SetEnabled(bEnable);
}

void PlayerLoreGlobals::SetSelectionMode(const SelectionMode eMode)
{
    if (m_eSelectionMode == eMode)
        return;

    m_eSelectionMode = eMode;

    switch (eMode)
    {
    case SLOT_SELECTION_MODE:
        EnableRingUI(TRUE);
        UI_SetControllerFocus(UIWND_LORE_CONSOLE_RING);
        break;

    case LORE_SELECTION_MODE:
        EnableRingUI(FALSE);
        UI_SetControllerFocus(UILIST_LORE_CONSOLE);
        break;
    }

    UILore::RefreshUI();
    RefreshUIForSelectionMode();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerLoreGlobals::RefreshUIForSelectionMode()
{
    UI_GetWindowFromID(UIWND_LORE_CONSOLE_RING_GLOW).SetActive(m_eSelectionMode == SLOT_SELECTION_MODE);
    UI_GetWindowFromID(UIWND_LORE_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(m_eSelectionMode != SLOT_SELECTION_MODE);
}

inline void PlayerLoreGlobals::OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);

    if (m_eSelectionMode == LORE_SELECTION_MODE)
    {
        ::Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput(vecStick, FALSE);
        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_LORE_CONSOLE);

        switch (idRule)
        {
        case ::Console::RULE_STICK_UP:
            tListBox.SelectPreviousItem(TRUE);
            break;
        case ::Console::RULE_STICK_DOWN:
            tListBox.SelectNextItem(TRUE);
            break;
        }
    }
}

inline void PlayerLoreGlobals::Listen()
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_LORE_CONSOLE);
    if (tListBox.GetSelectedIndex() >= 0)
        UILore::PlayLore(m_ptCurrentListViewDataSource->snoCachedLore[tListBox.GetSelectedIndex()], ptPlayer);
}

inline void PlayerLoreGlobals::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon == GetUIIDNone())
        return;

    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);

    if (bSelected)
    {
        UIUtils::Console::ResetScrollBox(UISCROLL_LORE_CONSOLE_SUMMARY);
        UILore::RefreshUI();
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void InitializeForGame()
{
    sgptLoreGlobals = new UILore::Console::PlayerLoreGlobals();
}

void DisposeForGame()
{
    delete sgptLoreGlobals;
    sgptLoreGlobals = NULL;
}

inline BOOL IsActive()
{
    return UI_GetWindowFromID(UIDLG_LORE_CONSOLE).IsActive();
}

void sUnMarkAsNew(const int32 nOldVal, const int32 nNewVal)
{
    ListViewDataSource * ptDataSource = sgptLoreGlobals->m_ptCurrentListViewDataSource;
    const int32 nNumItems = ptDataSource->GetNumItems();

    if (nNumItems == 1)
    {
        if (nNewVal >= 0 && nNewVal < nNumItems)
        {
            SNO snoLore = ptDataSource->snoCachedLore[nNewVal];
            sgptLoreGlobals->m_tNewLoreMgr.UnMarkAsNew(snoLore);
        }
    }
    else if (nOldVal != -1 && nNewVal != -1)
    {
        SNO snoOldLore = ptDataSource->snoCachedLore[nOldVal];
        sgptLoreGlobals->m_tNewLoreMgr.UnMarkAsNew(snoOldLore);
    }
}

void sRefreshForSelection()
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_LORE_CONSOLE);
    const int32 nSelectedIndex = tListBox.GetSelectedIndex();
    UIText & tTxtSummary = UI_GetTextFromID(UITXT_LORE_CONSOLE_SUMMARY);

    if (nSelectedIndex < 0)
    {
        tTxtSummary.SetText(StringListFetch("ConsoleQuest:NoJournal").str(), FALSE);
    }
    else
    {
        ListViewDataSource * ptDataSource = sgptLoreGlobals->m_ptCurrentListViewDataSource;
        SNO snoLore = ptDataSource->snoCachedLore[nSelectedIndex];
        ScopedLoreDefinitionLockR pdefLore(snoLore);
        CRefString sConvText = Conversation_GetFirstLine(pdefLore->snoConversation, sgptLoreGlobals->ptPlayer);
        tTxtSummary.SetText(sConvText, FALSE);
    }
}

static void sDisposeLoreControls()
{
    UILore::StopLore();
}

void OpenForController(int32 nControllerOpening)
{
    if (!IsActive() && nControllerOpening != -1 && PlayerGetByControllerIndex(nControllerOpening) != NULL)
    {
        Player * ptPlayerOpening = PlayerGetByControllerIndex(nControllerOpening);
        sgptLoreGlobals->OnOpen(ptPlayerOpening);

        UIID idInitialSelection = sUIIDForSlot(SLOT_OBJECTIVE);
        sgptLoreGlobals->m_tRingUI.SelectControl(idInitialSelection, IconSelectionChanged);

        ::Console::Screens::ShowModalDialog(nControllerOpening, UIDLG_LORE_CONSOLE, FALSE, GetUIIDNone());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        UIInventory::Console::UpdateNavBarTabState(ptPlayerOpening, UIDLG_LORE_CONSOLE, "LayoutRoot.Navigation", 1);
        UIUtils::Console::ResetScrollBox(UISCROLL_LORE_CONSOLE_SUMMARY);
        Refresh();
    }
}

void Close()
{
    sDisposeLoreControls();
    if (IsActive())
        ::Console::Screens::CloseModalDialog(UIDLG_LORE_CONSOLE);
}

void OnUpdate(const UIID &)
{
    if (sgptLoreGlobals == NULL)
        return;

    if (!sgptLoreGlobals->IsActive())
        return;

    sgptLoreGlobals->OnUpdate();
}

static void sIterateEventsCallback(QuestInstance & tQuestInst, void * pUserData)
{
    SNOArray * parEventSNOs = (SNOArray *)pUserData;

    ScopedQuestDefinitionLockR pdefQuest(tQuestInst.snoQuest);
    if (pdefQuest == NULL)
        return;

    if (pdefQuest->eQuestType != QUEST_TYPE_EVENT)
        return;

    if (tQuestInst.eInstanceState == QUEST_INSTANCE_ACTIVE &&
        tQuestInst.snoLevelArea == SNO_NONE)
    {
        parEventSNOs->Add_Tracked(tQuestInst.snoQuest);
    }
}

static void sRefreshObjective()
{
    enum QuestObjectiveType {
        OBJECTIVE_NORMAL = 0,
        OBJECTIVE_BOUNTY = 1,
        OBJECTIVE_RIFT = 2,
        OBJECTIVE_GREATER_RIFT = 3,
    };

    QuestObjectiveType eQuestType = OBJECTIVE_NORMAL;
    QuestInstance * ptQuestInst = NULL;

    UI_GetWindowFromID(UIWND_LORE_CONSOLE_QUEST_PANEL).SetActive(TRUE);

    if (GameIsOpenWorld())
    {
        ActorCommonData & tACDPlayer = ACDGet(sgptLoreGlobals->ptPlayer->idPlayerACD);
        SNO snoLevelArea = ACDGetLevelArea(tACDPlayer);
        if (LevelAreaGetDungeonFinderLevel(snoLevelArea) != 0)
        {
            SNO snoQuest = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
            ptQuestInst = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
            if (ptQuestInst != NULL)
            {
                GameCommonData & tGCD = CGameGetGameCommonData();
                if (tGCD.tGameSyncedData.nOpenLootRunLevel >= 0)
                    eQuestType = OBJECTIVE_GREATER_RIFT;
                else
                    eQuestType = OBJECTIVE_RIFT;
            }
        }
        else
        {
            SNO snoBounty = QuestGetActiveBountyForLevelArea(snoLevelArea, TRUE);
            ptQuestInst = QuestsGetQuestInstance(snoBounty, ACDID_INVALID);
            if (ptQuestInst != NULL)
                eQuestType = OBJECTIVE_BOUNTY;
        }
    }
    else
    {
        ptQuestInst = GetLeadingMainLineQuest();
    }

    if (ptQuestInst != NULL)
    {
        if (eQuestType == OBJECTIVE_BOUNTY && ptQuestInst->eInstanceState == QUEST_INSTANCE_COMPLETED)
        {
            SNOArray arEvents;
            QuestsIterate(sIterateEventsCallback, &arEvents);
            if (arEvents.GetArraySize() > 0)
                UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetText(StringListFetch("ConsoleQuest:AllBountiesComplete"), FALSE);
            else
                UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetText(StringListFetch("ConsoleQuest:BountyComplete"), FALSE);

            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY_HEADER).SetActive(FALSE);
        }
        else if (ptQuestInst->nCurrentStepUID != QUEST_UNASSIGNED_STEP_UID)
        {
            CRefString sObjective = UIQuests::GetCurrentObjectiveText(ptQuestInst);
            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetFontBoostSize(ObjectiveFontBoostSize);
            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetText(sObjective, FALSE);

            PlayerClass eClass = PlayerGetPlayerClass(sgptLoreGlobals->ptPlayer->idPlayerACD);
            CRefString sDescription = QuestStepGetDescription(ptQuestInst->snoQuest, ptQuestInst->nCurrentStepUID, eClass);
            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY).SetFontBoostSize(ObjectiveFontBoostSize);
            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY).SetText(sDescription, FALSE);

            UI_GetWindowFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY_HEADER).SetActive(sDescription.Length() != 0);
            UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY_HEADER).SetFontBoostSize(ObjectiveFontBoostSize);
        }
    }
    else
    {
        CRefString sNoQuests = StringListFetch("QuestLog:NoQuests");
        UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetFontBoostSize(4);
        UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE).SetText(sNoQuests.str(), FALSE);
        UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY).SetText("", FALSE);
        UI_GetWindowFromID(UITXT_LORE_CONSOLE_OBJECTIVE_SUMMARY_HEADER).SetActive(FALSE);
    }

    CRefString sObjectiveHeader;
    switch (eQuestType)
    {
    case OBJECTIVE_BOUNTY:
        sObjectiveHeader = StringListFetch("ConsoleQuest:Bounty");
        break;
    case OBJECTIVE_RIFT:
        sObjectiveHeader = StringListFetch("ConsoleQuest:Rift");
        break;
    case OBJECTIVE_GREATER_RIFT:
        sObjectiveHeader = StringListFetch("ConsoleQuest:GreaterRift");
        break;
    default:
        sObjectiveHeader = StringListFetch("ConsoleQuest:QuestObjective");
        break;
    }

    UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_HEADER).SetFontBoostSize(ObjectiveFontBoostSize);
    UI_GetTextFromID(UITXT_LORE_CONSOLE_OBJECTIVE_HEADER).SetText(sObjectiveHeader.str(), FALSE);

    CRefString sObjective = StringListFetch("ConsoleQuest:QuestObjective");
    UI_GetTextFromID(UITXT_LORE_CONSOLE_TITLE).SetText(sObjective.str(), FALSE);
    UI_GetTextFromID(UITXT_LORE_CONSOLE_TYPE).SetText(sObjective.str(), FALSE);
    UI_GetTextFromID(UITXT_LORE_CONSOLE_TYPE_DESCRIPTION).SetText(StringListFetch("ConsoleQuest:QuestObjectiveDescription"), FALSE);
}

static void sRefreshLore(int32 nSlot)
{
    const Slot eSlot = sgptLoreGlobals->GetSelectedSlot();
    if (eSlot == SLOT_NONE || eSlot == SLOT_OBJECTIVE)
        return;

    UI_GetWindowFromID(UIWND_LORE_CONSOLE_QUEST_PANEL).SetActive(FALSE);

    {
        LPCSTR szCategoryLabels[5] = { "Latest", "Quest", "World", "People", "Bestiary" };

        CRefString sCategory = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LORE_UI), szCategoryLabels[(eSlot >= SLOT_BEGIN && eSlot < SLOT_OBJECTIVE) ? eSlot : SLOT_MOST_RECENT]);
        UI_GetTextFromID(UITXT_LORE_CONSOLE_TITLE).SetText(sCategory, FALSE);
        UI_GetTextFromID(UITXT_LORE_CONSOLE_TYPE).SetText(sCategory, FALSE);

        CRefString sDescription = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LORE_UI), CRefString().Format("%s_Description_con", szCategoryLabels[(eSlot >= SLOT_BEGIN && eSlot < SLOT_OBJECTIVE) ? eSlot : SLOT_MOST_RECENT]));
        UI_GetTextFromID(UITXT_LORE_CONSOLE_TYPE_DESCRIPTION).SetText(sDescription, FALSE);
    }

    sgptLoreGlobals->RefreshList();
}

void Refresh()
{
    if (sgptLoreGlobals == NULL || !sgptLoreGlobals->IsActive())
        return;

    const int32 nSlot = sgptLoreGlobals->GetSelectedSlot();
    if (nSlot == SLOT_OBJECTIVE)
        sRefreshObjective();
    else
        sRefreshLore(nSlot);
}

void OnListSelectionChanged(const UIID &, const int32 nOldVal, const int32 nNewVal)
{
    sRefreshForSelection();
    sUnMarkAsNew(nOldVal, nNewVal);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    if (sgptLoreGlobals == NULL)
        return;

    if (!sgptLoreGlobals->IsActive())
        return;

    sgptLoreGlobals->OnLeftStick(id, nControllerIndex);
}

void OnButton1(const UIID &, const int32)
{
    const ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(4);
    switch (idAction)
    {
    case ACTION_OPEN_SLOT:
        sgptLoreGlobals->SetSelectionMode(PlayerLoreGlobals::LORE_SELECTION_MODE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        break;

    case ACTION_LISTEN:
        sgptLoreGlobals->Listen();
        break;
    }
}

void OnButton2(const UIID &, const int32)
{
    const ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(7);
    switch (idAction)
    {
    case ACTION_CLOSE_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        sgptLoreGlobals->SetSelectionMode(PlayerLoreGlobals::SLOT_SELECTION_MODE);
        break;

    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(1, 0);
        break;
    }
}

void OnButtonL2(const UIID &, const int32 nControllerIndex)
{
    if (sgptLoreGlobals->ptPlayer == NULL || sgptLoreGlobals->ptPlayer->nControllerIndex != nControllerIndex)
        return;

    if (::Console::Screens::GetActionManager().GetActionForInput(10) == ACTION_SCROLL && !::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
    {
        const UIID & idScrollBox = sgptLoreGlobals->GetSelectedSlot() == SLOT_OBJECTIVE ? UISCROLL_LORE_CONSOLE_OBJECTIVE_SUMMARY : UISCROLL_LORE_CONSOLE_SUMMARY;
        UIUtils::Console::ScrollContentInScrollBox(idScrollBox, -4.0f);
    }
}

void OnButtonR2(const UIID &, const int32 nControllerIndex)
{
    if (sgptLoreGlobals->ptPlayer == NULL || sgptLoreGlobals->ptPlayer->nControllerIndex != nControllerIndex)
        return;

    if (::Console::Screens::GetActionManager().GetActionForInput(11) == ACTION_SCROLL && !::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
    {
        const UIID & idScrollBox = sgptLoreGlobals->GetSelectedSlot() == SLOT_OBJECTIVE ? UISCROLL_LORE_CONSOLE_OBJECTIVE_SUMMARY : UISCROLL_LORE_CONSOLE_SUMMARY;
        UIUtils::Console::ScrollContentInScrollBox(idScrollBox, 4.0f);
    }
}

void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)
{
    sgptLoreGlobals->OnIconSelectionChanged(idIcon, bSelected);
}

inline Slot SlotFromIcon(const UIID & idIcon)
{
    return (Slot)(intptr_t)UI_GetWindowFromID(idIcon).GetUserData();
}

void OnLoreAcquired(const SNO snoLore)
{
    sgptLoreGlobals->m_tNewLoreMgr.MarkAsNew(snoLore);
}

}  // namespace Console
}  // namespace UILore
