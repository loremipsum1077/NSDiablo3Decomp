#include "UIConsoleListboxControl.h"

#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UICreation.h"
#include "../UIInit.h"
#include "UIConsoleScrollIndicatorControl.h"
#include "UIStackPanelControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
#include "../../Input/Input.h"
typedef int32 InputMsg;

















// X/Common/GameSwig.h







// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bClamped);
BOOL RStickIsInDeadZone(int32 nControllerIndex);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bClamped);
Vector2D GetRStickPosition(int32 nControllerIndex);
}  // namespace ControllerUtils
}  // namespace Console
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
class UIStackPanel;
class UIConsoleScrollIndicator;
BOOL UI_ValidateController(int32 nController);
const UIID & UI_GetControllerFocus();
void UI_RegisterInputListener(UIControl * pControl);
void UI_FlagInputListenerArrayDirty();
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIConsoleScrollIndicator & UI_GetConsoleScrollIndicatorFromID(const UIID & id);
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot); void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // UI.cpp:2866 overload, called at 0x710029d22c
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIListBoxChangeFunctionPtr UIListBoxChangeFunction_Lookup(LPCSTR szFunction);

// s_nSelectionDistanceThreshold, s_nAutoSelectionDistanceThreshold,
// s_nAutoSelectionMinimumVelocity and s_nBrakingAccelerationMagnitude (lines 28-32) are
// in the DWARF but the linker dropped them; their values are unknown.

struct RecyclableItem {  // line 44
    BOOL bInUse;  // 0x00
    uint64 nKey;  // 0x08
    UIID idItem;  // 0x10
};
static_assert(sizeof(RecyclableItem) == 0x18, "RecyclableItem");

ConsoleListBoxCreationParams::ConsoleListBoxCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

void ConsoleListBoxCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);

    pfnOnSelChange = UIListBoxChangeFunction_Lookup(pControlData->GetAttributeValue(ATT_ONSELCHANGE));
    bRequiresControllerFocus = pControlData->GetBoolValue(ATT_REQUIRES_CONTROLLER_FOCUS);
    bOnlyScrollWithDPad = pControlData->GetBoolValue(ATT_ONLY_SCROLL_WITH_DPAD);
    bScrollWithRightStick = pControlData->GetBoolValue(ATT_SCROLL_WITH_RSTICK);
    nStickSpeedLimitMS = pControlData->GetInt32Value(ATT_STICK_SPEED_LIMIT);
    nStackSpacing = pControlData->GetInt32Value(ATT_STACK_SPACING);
    eScrollbarVisibility = (ScrollbarVisibility)pControlData->GetInt32Value(ATT_SCROLLBAR_VISIBILITY);
    flScrollIndicatorPaddingTop = pControlData->GetFloat32Value(ATT_SCROLLINDICATOR_PADDING_TOP);
    flScrollIndicatorPaddingRight = pControlData->GetFloat32Value(ATT_SCROLLINDICATOR_PADDING_RIGHT);
    flScrollIndicatorPaddingBottom = pControlData->GetFloat32Value(ATT_SCROLLINDICATOR_PADDING_BOTTOM);
    snoMouseoverSound = SNOFileLookupGUID(SOUND_SNO, pControlData->GetAttributeValue(ATT_MOUSEOVER_SOUND));
    eStyle = pControlData->GetInt32Value(ATT_LIST_STYLE);
    bSupportsTouch = pControlData->GetBoolValue(ATT_SUPPORTS_TOUCH);
}

inline void ConsoleListBoxCreationParams::sInit()
{
    const uint32 DEFAULT_STICK_LIMIT_MS = 200;

    pfnOnSelChange = NULL;
    bRequiresControllerFocus = TRUE;
    bOnlyScrollWithDPad = FALSE;
    bScrollWithRightStick = FALSE;
    nStickSpeedLimitMS = DEFAULT_STICK_LIMIT_MS;
    nStackSpacing = 0;
    eScrollbarVisibility = SBV_HIDE_WHEN_NOT_NEEDED;
    flScrollIndicatorPaddingTop = 0.0f;
    flScrollIndicatorPaddingRight = 0.0f;
    flScrollIndicatorPaddingBottom = 0.0f;
    snoMouseoverSound = -1;
    eStyle = CONSOLE_LISTBOX_STYLE_LIST_VERTICAL;
}

UIConsoleListBox::UIConsoleListBox(const ConsoleListBoxCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);
    sInit(tParams, vecTL, vecDim);
}

void UIConsoleListBox::sInit(const ConsoleListBoxCreationParams & tParams, const Vector2D &, const Vector2D &)
{
    m_nSelectedIndex = -1;
    m_nFirstVisibleRow = -1;
    m_nMaxVisibleItems = 0;
    m_pfnOnSelChange = tParams.pfnOnSelChange;
    m_bOnlyScrollWithDPad = tParams.bOnlyScrollWithDPad;
    m_bScrollWithRightStick = tParams.bScrollWithRightStick;
    m_pfnOnScrolledOffTop = NULL;
    m_pfnOnScrolledOffBottom = NULL;
    m_bRequiresControllerFocus = tParams.bRequiresControllerFocus;
    m_nInputSpeedLimitMS = tParams.nStickSpeedLimitMS;
    m_ptDataSource = NULL;
    m_flScrollIndicatorPaddingTop = tParams.flScrollIndicatorPaddingTop;
    m_flScrollIndicatorPaddingRight = tParams.flScrollIndicatorPaddingRight;
    m_flScrollIndicatorPaddingBottom = tParams.flScrollIndicatorPaddingBottom;
    m_snoMouseoverSound = tParams.snoMouseoverSound;

    m_idStackPanel = GetUIIDNone();
    m_idScrollIndicator = GetUIIDNone();

    if (!(m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT)))
    {
        ConsoleScrollIndicatorCreationParams tScrollerParams(UIT_CONSOLESCROLLINDICATOR, "_scroller", GetID());
        tScrollerParams.eScrollbarVisibility = tParams.eScrollbarVisibility;
        m_idScrollIndicator = CreateControl<UIConsoleScrollIndicator>(tScrollerParams, Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));
    }

    m_eStyle = tParams.eStyle;
    m_bSupportsTouch = tParams.bSupportsTouch;

    StackPanelCreationParams tStackPanelParams(UIT_STACKPANEL, "_stackpanel", GetID());
    tStackPanelParams.fStackHorizontally = (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL);
    tStackPanelParams.fReverseOrder = FALSE;
    tStackPanelParams.flSpacing = (float)tParams.nStackSpacing;
    m_idStackPanel = CreateControl<UIStackPanel>(tStackPanelParams, Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));

    PositionChildControls();

    switch (m_eStyle)
    {
    case CONSOLE_LISTBOX_STYLE_LIST_VERTICAL:
        m_tStickInputFilter = Console::GetDefaultStickInputFilter(Console::FILTER_VERTICAL);
        break;
    case CONSOLE_LISTBOX_STYLE_GRID:
        m_tStickInputFilter = Console::GetDefaultStickInputFilter(Console::FILTER_DIRECTIONAL);
        break;
    case CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL:
        m_tStickInputFilter = Console::GetDefaultStickInputFilter(Console::FILTER_HORIZONTAL);
        break;
    }
    m_tStickInputFilter.SetMinimumInputDelay(m_nInputSpeedLimitMS);

    m_nNumItemsPerRow = 1;
    m_nDirection = 0;
}

void UIConsoleListBox::SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim)
{
    Vector2D vecCurrentDim = m_vecDim;

    UIWindow::SetRelativePosition(vecTL, vecDim);

    if (Math::Abs(vecCurrentDim.x - m_vecDim.x) > 1e-6f ||
        Math::Abs(vecCurrentDim.y - m_vecDim.y) > 1e-6f)
    {
        PositionChildControls();
    }

    CalculateMaxNumberOfVisibleItems();
}

int32 UIConsoleListBox::GetNumItems() const
{
    // DWARF row 260:5/12/45: one return line, the virtual call at col 45 (a ternary)
    return m_ptDataSource ? m_ptDataSource->GetNumItems() : 0;
}





inline int32 UIConsoleListBox::GetNumRows() const
{
    if (m_nNumItemsPerRow == 0)
    {
        return 0;
    }

    if (m_nNumItemsPerRow == 1)
    {
        return GetNumItems();
    }

    {
        int32 nNumItems = GetNumItems();
        // DWARF has no nNumRows local (only nNumItems, 279); row 280:13/23/25/43 is this test
        if (nNumItems % m_nNumItemsPerRow != 0)
        {
            return (nNumItems / m_nNumItemsPerRow) + 1;
        }
        return nNumItems / m_nNumItemsPerRow;
    }
}

inline int32 UIConsoleListBox::GetNumItemsPerRow() const
{
    return m_nNumItemsPerRow;
}

int32 UIConsoleListBox::GetNumVisibleItems() const
{
    if (m_ptDataSource != NULL) // DWARF: nNumItems (308) sits in a block whose test has row 306:9; inferred as the data-source test
    {
        int32 nNumItems = GetNumItems();
        if (nNumItems > 0)
        {
            int32 nFirstVisibleItemIndex = GetFirstVisibleItemIndex();
            if (nFirstVisibleItemIndex != -1)
            {
                return Math::Min(m_nMaxVisibleItems, nNumItems - nFirstVisibleItemIndex);
            }

            return Math::Min(m_nMaxVisibleItems, nNumItems);
        }
    }
    return 0;
}

int32 UIConsoleListBox::GetNumVisibleRows() const
{
    int32 nNumItemsPerRow = GetNumItemsPerRow();
    if (nNumItemsPerRow == 0)
    {
        return 0;
    }

    if (nNumItemsPerRow == 1)
    {
        return GetNumVisibleItems();
    }

    {
        int32 nNumVisibleItems = GetNumVisibleItems();
        if (nNumVisibleItems == 0)
        {
            return 0;
        }

        {
            int32 nRemainder = nNumVisibleItems % nNumItemsPerRow;
            return nNumVisibleItems / nNumItemsPerRow + (nRemainder != 0 ? 1 : 0);
        }
    }
}

inline int32 UIConsoleListBox::GetRowForIndex(const int32 nIndex) const
{
    if (m_nNumItemsPerRow == 0 || nIndex == -1)
    {
        return -1;
    }

    return nIndex / m_nNumItemsPerRow;
}

void UIConsoleListBox::SetSelectedIndex(const int32 nIndex)
{
    UI_FlagInputListenerArrayDirty();

    if (m_ptDataSource == NULL || m_nSelectedIndex == nIndex || nIndex >= m_ptDataSource->GetNumItems())
    {
        return;
    }

    {
        int32 nOldSelectedIndex = m_nSelectedIndex;
        m_nSelectedIndex = nIndex;

        if (m_pfnOnSelChange != NULL)
        {
            m_pfnOnSelChange(GetID(), nOldSelectedIndex, nIndex);
        }

        if (nIndex != -1)
        {
            if (IsItemVisible(nIndex))
            {
                RefreshItem(nIndex);
            }
            else
            {
                ScrollItemIntoView(nIndex);
            }
        }

        if (nOldSelectedIndex != -1 && IsItemVisible(nOldSelectedIndex))
        {
            RefreshItem(nOldSelectedIndex);
        }
    }
}

int32 UIConsoleListBox::GetNextSelectableIndex() const
{
    int32 nSelectedIndex = GetSelectedIndex();

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
    {
        do
        {
            nSelectedIndex = Math::Pin(nSelectedIndex + 1, 0, GetNumItems() - 1);
        } while (nSelectedIndex != -1 && nSelectedIndex < GetNumItems() - 1 &&
                 m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex));
    }
    else
    {
        int32 nLastIndex = m_nNumItemsPerRow * (GetRowForIndex(nSelectedIndex) + 1) - 1;
        if (nSelectedIndex != nLastIndex)
        {
            nSelectedIndex++;
        }
    }

    return nSelectedIndex;
}

int32 UIConsoleListBox::GetPrevSelectableIndex() const
{
    int32 nSelectedIndex = GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return -1;
    }

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
    {
        do
        {
            nSelectedIndex = Math::Pin(nSelectedIndex - 1, 0, GetNumItems() - 1);
        } while (nSelectedIndex > 0 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex));
    }
    else
    {
        int32 nFirstIndex = m_nNumItemsPerRow * GetRowForIndex(nSelectedIndex);
        if (nSelectedIndex != nFirstIndex)
        {
            nSelectedIndex--;
        }
    }

    return nSelectedIndex;
}

void UIConsoleListBox::SetAnalogStickMinimumInputDelay(uint32 nInputSpeedLimitMS)
{
    m_tStickInputFilter.SetMinimumInputDelay(nInputSpeedLimitMS);
}

Blizzard::Time::Millisecond UIConsoleListBox::GetAnalogStickMinimumInputDelay()
{
    return m_nInputSpeedLimitMS;
}

void UIConsoleListBox::ResetAnalogMinimumInputDelay()
{
    m_tStickInputFilter.SetMinimumInputDelay(m_nInputSpeedLimitMS);
}

int32 UIConsoleListBox::GetSelectableIndexAbove() const
{
    int32 nSelectedIndex;
    int32 nRow;
    int32 nOffset;
    int32 nNewIndex;

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
    {
        return GetPrevSelectableIndex();
    }

    nSelectedIndex = GetSelectedIndex();
    if (nSelectedIndex == -1 || m_ptDataSource == NULL)
    {
        return -1;
    }

    nRow = GetRowForIndex(nSelectedIndex);
    if (nRow > 0)
    {
        nOffset = nSelectedIndex % m_nNumItemsPerRow;
        nNewIndex = nOffset + (nRow - 1) * m_nNumItemsPerRow;
        nSelectedIndex = m_ptDataSource->IsItemEnabled(*this, nNewIndex) ? nNewIndex : nSelectedIndex;
    }

    return nSelectedIndex;
}

int32 UIConsoleListBox::GetSelectableIndexBelow() const
{
    int32 nSelectedIndex;
    int32 nRow;
    int32 nNumItems;
    int32 nNewIndex;
    int32 nOffset;

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
    {
        return GetNextSelectableIndex();
    }

    nSelectedIndex = GetSelectedIndex();
    if (nSelectedIndex == -1 || m_ptDataSource == NULL)
    {
        return -1;
    }

    nRow = GetRowForIndex(nSelectedIndex);
    if (nRow < GetNumRows() - 1)
    {
        nOffset = nSelectedIndex - nRow * m_nNumItemsPerRow;
        nNewIndex = (nRow + 1) * m_nNumItemsPerRow + nOffset;
        nNumItems = m_ptDataSource->GetNumItems();
        if (nNewIndex >= nNumItems)
        {
            nNewIndex = nNumItems - 1;
        }
        nSelectedIndex = m_ptDataSource->IsItemEnabled(*this, nNewIndex) ? nNewIndex : nSelectedIndex;
    }

    return nSelectedIndex;
}

void UIConsoleListBox::SelectNextItem(BOOL bPlaySound)
{
    int32 nSelectedIndex = GetNextSelectableIndex();
    if (nSelectedIndex != -1 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex))
    {
        return;
    }

    int32 nOldIndex = GetSelectedIndex();
    if (nSelectedIndex != nOldIndex)
    {
        SetSelectedIndex(nSelectedIndex);

        if (bPlaySound)
        {
            PlaySelectionSound();
        }
    }
}

void UIConsoleListBox::SelectPreviousItem(BOOL bPlaySound)
{
    int32 nSelectedIndex = GetPrevSelectableIndex();
    if (nSelectedIndex != -1 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex))
    {
        return;
    }

    if (nSelectedIndex != GetSelectedIndex())
    {
        SetSelectedIndex(nSelectedIndex);

        if (bPlaySound)
        {
            PlaySelectionSound();
        }
    }
}

inline void UIConsoleListBox::SelectItemBelow(BOOL bPlaySound)
{
    int32 nSelectedIndex = GetSelectableIndexBelow();
    if (nSelectedIndex != -1 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex))
    {
        return;
    }

    int32 nOldIndex = GetSelectedIndex();
    if (nSelectedIndex != nOldIndex)
    {
        m_nDirection = 1;
        SetSelectedIndex(nSelectedIndex);

        if (bPlaySound)
        {
            PlaySelectionSound();
        }
    }
}

inline void UIConsoleListBox::SelectItemAbove(BOOL bPlaySound)
{
    int32 nSelectedIndex = GetSelectableIndexAbove();
    if (nSelectedIndex != -1 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, nSelectedIndex))
    {
        return;
    }

    int32 nOldIndex = GetSelectedIndex();
    if (nSelectedIndex != nOldIndex)
    {
        m_nDirection = -1;
        SetSelectedIndex(nSelectedIndex);

        if (bPlaySound)
        {
            PlaySelectionSound();
        }
    }
}

void UIConsoleListBox::SetFirstVisibleItemIndex(const int32 nIndex)
{
    int32 nRow = GetRowForIndex(nIndex);
    SetFirstVisibleRow(nRow);
}

inline void UIConsoleListBox::SetFirstVisibleRow(const int32 nRow)
{
    if (m_nFirstVisibleRow == nRow)
    {
        return;
    }

    {
        int32 nMaxVisibleRows = GetRowForIndex(m_nMaxVisibleItems - 1);
        int32 nNumRows = GetNumRows();

        if (nRow + nMaxVisibleRows > nNumRows && nNumRows >= nMaxVisibleRows)
        {
            m_nFirstVisibleRow = nNumRows - nMaxVisibleRows;
        }
        else
        {
            m_nFirstVisibleRow = Math::Max(0, nRow);
        }

        RefreshVisibleItems();
    }
}

// NON_MATCHING: 2 insns: the clamp to 0 tests the sign of the result plus one (`bic w20, w8, w8, asr #31`)
// where the original tests nRow - nNumVisibleRows (`bic w20, w9, w8, asr #31`). Max(0, ...) operand order (col 46) and the
// caller-side `!= m_nFirstVisibleRow` test at 761 (cols 9/32) fixed the swapped compare.
#line 747
void UIConsoleListBox::SetLastVisibleRow(
    const int32 nRow)
{
    int32 nNumRows = GetNumRows();
    int32 nNumVisibleRows = GetNumVisibleRows();

    if (nNumRows < nNumVisibleRows)
    {
        return;
    }



    int32 nNewFirstVisibleRow = Math::Max(0, nRow - nNumVisibleRows + 1);
    if (nNewFirstVisibleRow != m_nFirstVisibleRow)
    {
        SetFirstVisibleRow(nNewFirstVisibleRow);
    }
}
#line 596

inline BOOL UIConsoleListBox::IsItemVisible(const int32 nIndex) const
{
    return nIndex >= GetFirstVisibleItemIndex() &&
           nIndex <= GetLastVisibleItemIndex();
}

void UIConsoleListBox::ScrollItemIntoView(const int32 nItemIndex)
{
    if (nItemIndex < 0 || nItemIndex >= GetNumItems())
    {
        return;
    }

    int32 nRow = GetRowForIndex(nItemIndex);
    if (nRow < m_nFirstVisibleRow)
    {
        SetFirstVisibleItemIndex(nItemIndex);
    }
    else if (nRow > GetRowForIndex(GetLastVisibleItemIndex()))
    {
        SetLastVisibleRow(nRow);
    }
}

void UIConsoleListBox::ResetAllItems()
{
    if (m_aridVisibleItems.size() != 0)
    {
        m_tItemPool.ReturnMultipleItemsToPool(m_aridVisibleItems, 0, (int32)m_aridVisibleItems.size() - 1);

        if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
        {
            UI_GetStackPanelFromID(m_idStackPanel).RemoveAllChildren();
        }
        else
        {
            const UIControlItemArray & arChildren = UI_GetStackPanelFromID(m_idStackPanel).GetChildList();
            for (uint32 i = 0; i < arChildren.size(); i++)
            {
                UI_GetControlFromID(arChildren[i].id).SetActive(FALSE);
            }
        }

        m_aridVisibleItems.clear();
    }

    m_nFirstVisibleRow = -1;
    SetSelectedIndex(-1);
    m_nDirection = 0;

    {
        int32 nNumItems = GetNumItems();
        if (nNumItems > 0 && m_nMaxVisibleItems > 0)
        {
            m_nFirstVisibleRow = 0;
            CreateMultipleItems(0, Math::Min(m_nMaxVisibleItems, nNumItems) - 1);
            SelectNextItem(FALSE);
        }
    }
}

void UIConsoleListBox::RefreshItem(const int32 nIndex)
{
    if (IsItemVisible(nIndex) && m_ptDataSource != NULL)
    {
        const UIID & idItem = GetItemUIID(nIndex);
        if (UI_TestID(idItem))
        {
            m_ptDataSource->UpdateItemUI(*this, idItem, nIndex);
        }
    }
}

// NON_MATCHING: 2036 bytes against 2044. The original leaves the template-check loop with a
// branch on the first mismatching item and tests a constant flag afterwards; this version
// carries bDoesListNeedToBeRecreated through the loop (cset/csel) instead. The rest matches.
void UIConsoleListBox::RefreshVisibleItems()
{
    BOOL bDoesListNeedToBeRecreated = FALSE;
    int32 FirstItemToBeRemade = -1;
    UIControlSourceKey tItemTemplate;

    if (m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT))
    {
        CalculateMaxNumberOfVisibleItems();
    }

    if ((int32)m_aridVisibleItems.size() < GetNumVisibleItems())
    {
        CreateMultipleItems(m_aridVisibleItems.size(), GetNumVisibleItems() - 1);
    }

    if (m_nFirstVisibleRow == -1 && GetNumVisibleItems() > 0)
    {
        m_nFirstVisibleRow = 0;
    }

    if (m_nFirstVisibleRow != -1 && GetNumVisibleItems() <= 0)
    {
        m_nFirstVisibleRow = -1;
    }

    for (int32 i = GetFirstVisibleItemIndex(); i <= GetLastVisibleItemIndex() && !bDoesListNeedToBeRecreated; i++)
    {
        const RecyclableItem * pItemInPool = m_tItemPool.GetItemInfoByUIID(GetItemUIID(i));
        tItemTemplate = m_ptDataSource->GetSourceKeyForItem(i);
        if (pItemInPool->nKey != tItemTemplate.nKey)
        {
            bDoesListNeedToBeRecreated = TRUE;
            FirstItemToBeRemade = i;
        }
    }

    if (bDoesListNeedToBeRecreated)
    {
        RemoveMultipleItems(FirstItemToBeRemade, GetLastVisibleItemIndex());
        CreateMultipleItems(FirstItemToBeRemade, GetLastVisibleItemIndex());
    }

    for (int32 i = GetFirstVisibleItemIndex(); i <= GetLastVisibleItemIndex(); i++)
    {
        const UIID & idItem = GetItemUIID(i);
        m_ptDataSource->UpdateItemUI(*this, idItem, i);
    }

    if (m_nSelectedIndex >= GetNumItems())
    {
        SetSelectedIndex(GetNumItems() - 1);
    }

    if (GetSelectedIndex() != -1 && m_ptDataSource != NULL && !m_ptDataSource->IsItemEnabled(*this, m_nSelectedIndex))
    {
        if (m_nSelectedIndex > 0)
        {
            SelectPreviousItem(TRUE);
        }
        else
        {
            SelectNextItem(TRUE);
        }
    }

    while ((int32)m_aridVisibleItems.size() > GetNumVisibleItems())
    {
        m_tItemPool.ReturnItemToPool(m_aridVisibleItems.back());
        m_aridVisibleItems.pop_back();
    }
}

void UIConsoleListBox::SetDataSource(ConsoleListBoxDataSource & tDataSource)
{
    if (m_ptDataSource == &tDataSource)
    {
        return;
    }

    m_ptDataSource = &tDataSource;
    m_nFirstVisibleRow = tDataSource.GetNumItems() > 0 ? 0 : -1;
    m_nSelectedIndex = -1;
    CalculateMaxNumberOfVisibleItems();
    ResetAllItems();
}

void UIConsoleListBox::SimulateStickInput(BOOL bStickUp)
{
    Vector2D vecStick(0.0f, bStickUp ? 1.0f : -1.0f);
    m_tStickInputFilter.HandleInput(vecStick, FALSE);
}

void UIConsoleListBox::LostControllerFocus()
{
    int32 nIndex = GetSelectedIndex();
    if (nIndex != -1)
    {
        const UIID & idItem = GetItemUIID(nIndex);
        if (UI_TestID(idItem))
        {
            m_ptDataSource->UpdateItemUI(*this, idItem, nIndex);
        }
    }
}

void UIConsoleListBox::GainedControllerFocus()
{
    int32 nIndex = GetSelectedIndex();
    if (nIndex != -1)
    {
        const UIID & idItem = GetItemUIID(nIndex);
        if (UI_TestID(idItem))
        {
            m_ptDataSource->UpdateItemUI(*this, idItem, nIndex);
        }
    }
}

BOOL UIConsoleListBox::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_INPUT:
        {
            const BufferedInput & tInput = *(const BufferedInput *)dwParam;
            if (UI_ValidateController(tInput.tBufControllerData.nControllerIndex) && HandleInput(tInput))
            {
                return TRUE;
            }
        }
        break;

    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener(this);
        break;

    case UI_UPDATE:
        if (!(m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT)))
        {
            int32 nNumVisibleRows;
            int32 nScrollRange;
            int32 nNumRows = GetNumRows();
            nNumVisibleRows = GetNumVisibleRows();
            nScrollRange = Math::Max(nNumRows - nNumVisibleRows, 0);
            UI_GetConsoleScrollIndicatorFromID(m_idScrollIndicator).SetExtentsAndPosition(nScrollRange, m_nFirstVisibleRow);
        }
        break;

    default:
        break;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}

BOOL UIConsoleListBox::ForwardMessage(const UIMsg & msg, const uintptr dwParam)
{
    if (msg == UI_INPUT || msg == UI_REGISTER_INPUT_LISTENER)
    {
        if (m_nSelectedIndex >= 0 && GetNumItems() > 0 && UI_TestID(GetItemUIID(m_nSelectedIndex)))
        {
            if (UI_GetWindowFromID(GetItemUIID(m_nSelectedIndex)).HandleMessage(msg, dwParam))
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    return UIWindow::ForwardMessage(msg, dwParam);
}

inline BOOL UIConsoleListBox::HandleInput(const BufferedInput & tInput)
{
    switch (tInput.eInputMsg)
    {
    case INPUTMSG_LEFTSTICK:
        if (!m_bOnlyScrollWithDPad && !m_bScrollWithRightStick)
        {
            return HandleStickInput(tInput);
        }
        break;

    case INPUTMSG_RIGHTSTICK:
        if (!m_bOnlyScrollWithDPad && m_bScrollWithRightStick)
        {
            return HandleStickInput(tInput);
        }
        break;

    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
    case INPUTMSG_CONTROLLERBUTTONUP:
        switch (tInput.tBufControllerData.eInputButton)
        {
        case BUTTON_DPAD_UP:
        case BUTTON_DPAD_DOWN:
        case BUTTON_DPAD_LEFT:
        case BUTTON_DPAD_RIGHT:
            return HandleDPadInput(tInput);
        }
        break;
    }

    return FALSE;
}

BOOL UIConsoleListBox::HandleStickInput(const BufferedInput & tInput)
{
    BOOL bInDeadZone;
    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();
    BOOL bRightStick = tInput.eInputMsg == INPUTMSG_RIGHTSTICK && m_bScrollWithRightStick;

    bInDeadZone = bRightStick ? Console::ControllerUtils::RStickIsInDeadZone(tInput.tBufControllerData.nControllerIndex)
                              : Console::ControllerUtils::LStickIsInDeadZone(tInput.tBufControllerData.nControllerIndex, FALSE);

    if ((!bHasControllerFocus && m_bRequiresControllerFocus) || bInDeadZone)
    {
        return FALSE;
    }

    {
        Vector2D vecStick = bRightStick ? Console::ControllerUtils::GetRStickPosition(tInput.tBufControllerData.nControllerIndex)
                                        : Console::ControllerUtils::GetLStickPosition(tInput.tBufControllerData.nControllerIndex, FALSE);
        Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput(vecStick, FALSE);

        switch (idRule)
        {
        case Console::AnalogStickInputFilter::Rule::IGNORE_INPUT:
            {
                Console::AnalogStickInputFilter::Rule::ID idNoWaitRule = m_tStickInputFilter.HandleInput(vecStick, TRUE);
                if (idNoWaitRule == Console::RULE_STICK_UP || idNoWaitRule == Console::RULE_STICK_DOWN)
                {
                    return TRUE;
                }
            }
            break;

        case Console::RULE_STICK_DOWN:
            {
                int32 nPrevSelectedIndex = GetSelectedIndex();
                SelectItemBelow(TRUE);
                if (nPrevSelectedIndex == GetSelectedIndex() && m_pfnOnScrolledOffBottom != NULL)
                {
                    m_pfnOnScrolledOffBottom(GetID());
                }
            }
            return TRUE;

        case Console::RULE_STICK_UP:
            {
                int32 nPrevSelectedIndex = GetSelectedIndex();
                SelectItemAbove(TRUE);
                if (nPrevSelectedIndex == GetSelectedIndex() && m_pfnOnScrolledOffTop != NULL)
                {
                    m_pfnOnScrolledOffTop(GetID());
                }
            }
            return TRUE;

        case Console::RULE_STICK_LEFT:
            SelectPreviousItem(TRUE);
            break;

        case Console::RULE_STICK_RIGHT:
            SelectNextItem(TRUE);
            break;
        }
    }

    return FALSE;
}

BOOL UIConsoleListBox::HandleDPadInput(const BufferedInput & tInput)
{
    static Console::InputSpeedLimiter tSpeedLimiter;
    static InputButton eLastButton = BUTTON_INVALID;

    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP)
    {
        tSpeedLimiter.ResetLastInputTime();
        return FALSE;
    }

    BOOL bShouldEatInput = FALSE;
    BOOL bIgnoreInput = !tSpeedLimiter.HasWaitTimeExpired(m_nInputSpeedLimitMS);
    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();

    if (bHasControllerFocus || !m_bRequiresControllerFocus)
    {
        switch (m_eStyle)
        {
        case CONSOLE_LISTBOX_STYLE_LIST_VERTICAL:
            bShouldEatInput = tInput.tBufControllerData.eInputButton == BUTTON_DPAD_UP ||
                              tInput.tBufControllerData.eInputButton == BUTTON_DPAD_DOWN;
            break;
        case CONSOLE_LISTBOX_STYLE_GRID:
            bShouldEatInput = tInput.tBufControllerData.eInputButton == BUTTON_DPAD_UP ||
                              tInput.tBufControllerData.eInputButton == BUTTON_DPAD_DOWN ||
                              tInput.tBufControllerData.eInputButton == BUTTON_DPAD_LEFT ||
                              tInput.tBufControllerData.eInputButton == BUTTON_DPAD_RIGHT;
            break;
        case CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL:
            bShouldEatInput = tInput.tBufControllerData.eInputButton == BUTTON_DPAD_LEFT ||
                              tInput.tBufControllerData.eInputButton == BUTTON_DPAD_RIGHT;
            break;
        }

        if (GetNumItems() == 0)
        {
            return bHasControllerFocus && bShouldEatInput;
        }

        if (eLastButton != tInput.tBufControllerData.eInputButton)
        {
            bIgnoreInput = FALSE;
        }
        eLastButton = tInput.tBufControllerData.eInputButton;

        if (!bIgnoreInput && bShouldEatInput)
        {
            if (m_eStyle == CONSOLE_LISTBOX_STYLE_GRID && tInput.tBufControllerData.eInputButton == BUTTON_DPAD_LEFT)
            {
                SelectPreviousItem(TRUE);
            }
            else if (m_eStyle == CONSOLE_LISTBOX_STYLE_GRID && tInput.tBufControllerData.eInputButton == BUTTON_DPAD_RIGHT)
            {
                SelectNextItem(TRUE);
            }
            else
            {
                switch (tInput.tBufControllerData.eInputButton)
                {
                case BUTTON_DPAD_UP:
                case BUTTON_DPAD_LEFT:
                    {
                        int32 nPreviousRow = GetRowForIndex(GetSelectedIndex());
                        SelectItemAbove(TRUE);
                        if (nPreviousRow == GetRowForIndex(GetSelectedIndex()) && m_pfnOnScrolledOffTop != NULL)
                        {
                            m_pfnOnScrolledOffTop(GetID());
                        }
                    }
                    break;

                case BUTTON_DPAD_DOWN:
                case BUTTON_DPAD_RIGHT:
                    {
                        int32 nPreviousRow = GetRowForIndex(GetSelectedIndex());
                        SelectItemBelow(TRUE);
                        if (nPreviousRow == GetRowForIndex(GetSelectedIndex()) && m_pfnOnScrolledOffBottom != NULL)
                        {
                            m_pfnOnScrolledOffBottom(GetID());
                        }
                    }
                    break;
                }
            }

            tSpeedLimiter.UpdateLastInputTime();
            return TRUE;
        }
    }

    return bHasControllerFocus && bShouldEatInput;
}

const UIID & UIConsoleListBox::GetItemUIID(const int32 nIndex) const
{
    int32 nSlotIndex = nIndex - GetFirstVisibleItemIndex();
    if (nSlotIndex >= 0 && nSlotIndex < (int32)m_aridVisibleItems.size())
    {
        return m_aridVisibleItems[nSlotIndex];
    }

    return GetUIIDNone();
}

void UIConsoleListBox::CalculateMaxNumberOfVisibleItems()
{
    float flItemWidth;
    float flItemHeight;

    if (m_ptDataSource == NULL)
    {
        m_nMaxVisibleItems = 0;
        return;
    }

    if (m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT))
    {
        m_nMaxVisibleItems = GetNumItems();
        return;
    }

    {
        UIControlSourceKey tItemTemplate = m_ptDataSource->GetSourceKeyForItem(-1);
        UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tItemTemplate, DynamicControlCreationParams("temp slot list", cgUIIDRoot), NULL);
        Vector2D vecTempSlotTL;
        Vector2D vecTempSlotDim;
        ptWindow->GetRelativePosition(vecTempSlotTL, vecTempSlotDim);
        UI_DeleteControl(ptWindow->GetID(), TRUE);
        flItemWidth = vecTempSlotDim.x;
        flItemHeight = vecTempSlotDim.y;
    }

    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);

    UIStackPanel & tStackPanel = UI_GetStackPanelFromID(m_idStackPanel);

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_VERTICAL || m_eStyle == CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL)
    {
        m_nNumItemsPerRow = 1;
    }
    else
    {
        m_nNumItemsPerRow = (int32)(vecDim.x / (flItemWidth + tStackPanel.GetSpacing()));
    }

    m_nMaxVisibleItems = (int32)(vecDim.y / (flItemHeight + tStackPanel.GetSpacing())) * m_nNumItemsPerRow;
}

void UIConsoleListBox::CreateItem(const int32 nItemIndex)
{
    if (m_ptDataSource == NULL)
    {
        return;
    }

    BOOL bNewlyCreatedItem = FALSE;
    UIID idParentStackPanel = m_idStackPanel;

    if (m_eStyle == CONSOLE_LISTBOX_STYLE_GRID)
    {
        int32 nRow = GetRowForIndex(nItemIndex);
        idParentStackPanel = GetGridRowStackPanelId(nRow);
        if (!UI_TestID(idParentStackPanel))
        {
            idParentStackPanel = CreateGridRowStackPanel(nRow);
        }
        else
        {
            UI_GetControlFromID(idParentStackPanel).SetActive(TRUE);
        }
    }

    UIControlSourceKey tItemTemplate = m_ptDataSource->GetSourceKeyForItem(nItemIndex);
    UIID idItem = m_tItemPool.ReserveItemFromPool(tItemTemplate.nKey, idParentStackPanel);
    if (idItem == GetUIIDNone())
    {
        idItem = m_tItemPool.AddItemToPool(tItemTemplate, idParentStackPanel);
        bNewlyCreatedItem = TRUE;
    }

    m_aridVisibleItems.push_back(idItem);

    if (bNewlyCreatedItem)
    {
        m_ptDataSource->InitializeItemUI(*this, idItem, nItemIndex);
    }

    m_ptDataSource->UpdateItemUI(*this, idItem, nItemIndex);
}

inline void UIConsoleListBox::CreateMultipleItems(const int32 nStartingIndex, const int32 nEndingIndex)
{
    for (int32 nIndex = nStartingIndex; nIndex <= nEndingIndex; nIndex++)
    {
        CreateItem(nIndex);
    }
}

void UIConsoleListBox::RemoveMultipleItems(const int32 nStartingIndex, const int32 nEndingIndex)
{
    int32 nAdjustedStartingIndex = nStartingIndex - GetFirstVisibleItemIndex();
    int32 nAdjustedEndingIndex = nEndingIndex - GetFirstVisibleItemIndex();

    m_tItemPool.ReturnMultipleItemsToPool(m_aridVisibleItems, nAdjustedStartingIndex, nAdjustedEndingIndex);

    for (int32 i = nAdjustedEndingIndex; i >= nAdjustedStartingIndex; i--)
    {
        blzext::vector_erase_at(m_aridVisibleItems, i);
    }
}

void UIConsoleListBox::PositionChildControls()
{
    if (m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT))
    {
        return;
    }

    UIConsoleScrollIndicator & tScroller = UI_GetConsoleScrollIndicatorFromID(m_idScrollIndicator);
    Vector2D vecThumbSize;
    tScroller.GetThumbSize(vecThumbSize);

    if (UI_TestID(m_idStackPanel))
    {
        UIWindow & tStackPanel = UI_GetWindowFromID(m_idStackPanel);
        Vector2D vecStackPanelTL;
        Vector2D vecStackPanelDim;
        tStackPanel.GetRelativePosition(vecStackPanelTL, vecStackPanelDim);
        vecStackPanelDim.x = m_vecDim.x;
        vecStackPanelDim.y = Math::Max(0.0f, m_vecDim.y - vecThumbSize.x);
        tStackPanel.SetRelativePosition(vecStackPanelTL, vecStackPanelDim);
    }

    if (UI_TestID(m_idScrollIndicator))
    {
        Vector2D vecScrollerTL;
        Vector2D vecScrollerDim;
        vecScrollerTL.x = m_vecDim.x - vecThumbSize.x - m_flScrollIndicatorPaddingRight;
        vecScrollerTL.y = m_flScrollIndicatorPaddingTop;
        vecScrollerDim.x = vecThumbSize.x;
        vecScrollerDim.y = m_vecDim.y - m_flScrollIndicatorPaddingTop - m_flScrollIndicatorPaddingBottom;
        tScroller.SetRelativePosition(vecScrollerTL, vecScrollerDim);
    }
}

inline void UIConsoleListBox::PlaySelectionSound()
{
    if (m_snoMouseoverSound != -1)
    {
        SoundManager::PlaySound(m_snoMouseoverSound, cgtPSODefaults);
    }
}

UIID UIConsoleListBox::CreateGridRowStackPanel(const int32 nRow)
{
    UIID idRowStackPanel = GetUIIDNone();

    CRefString sRowId;
    sRowId.Format("_row%d", nRow);

    StackPanelCreationParams tStackPanelParams(UIT_STACKPANEL, sRowId.str(), m_idStackPanel);
    tStackPanelParams.fStackHorizontally = TRUE;
    tStackPanelParams.fReverseOrder = FALSE;
    tStackPanelParams.flSpacing = UI_GetStackPanelFromID(m_idStackPanel).GetSpacing();
    idRowStackPanel = CreateControl<UIStackPanel>(tStackPanelParams, Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));

    return idRowStackPanel;
}

inline UIID UIConsoleListBox::GetGridRowStackPanelId(const int32 nRow)
{
    CRefString sRowId;
    sRowId.Format("._row%d", nRow);
    return UIIDCreateFromParent(m_idStackPanel, sRowId.str());
}

inline RecyclableItemPool::~RecyclableItemPool()
{
    for (uint32 nIndex = 0; nIndex < m_arItemPool.size(); nIndex++)
    {
        UI_DeleteControl(m_arItemPool[nIndex].idItem, TRUE);
    }

    m_arItemPool.clear();
}

UIID RecyclableItemPool::AddItemToPool(const UIControlSourceKey & tItemTemplate, const UIID & idParent)
{
    uint32 nItemNumber = m_arItemPool.size();

    UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tItemTemplate, DynamicControlCreationParams(GetItemID(nItemNumber), idParent), NULL);

    RecyclableItem tItem;
    tItem.bInUse = TRUE;
    tItem.nKey = tItemTemplate.nKey;
    tItem.idItem = ptWindow->GetID();
    m_arItemPool.push_back(tItem);

    return tItem.idItem;
}

UIID RecyclableItemPool::ReserveItemFromPool(uint64 nKey, const UIID & idParent)
{
    for (uint32 nIndex = 0; nIndex < m_arItemPool.size(); nIndex++)
    {
        RecyclableItem & tItem = m_arItemPool[nIndex];
        if (tItem.nKey == nKey && !tItem.bInUse)
        {
            UIID idOldParent = UI_GetControlFromID(tItem.idItem).GetParent();
            if (idOldParent == idParent)
            {
                UI_GetControlGroupFromID(idOldParent).RemoveChild(tItem.idItem);
                UI_GetWindowFromID(tItem.idItem).SetActive(TRUE);
                UI_GetControlGroupFromID(idParent).AddChild(tItem.idItem);
                tItem.bInUse = TRUE;
                return tItem.idItem;
            }
        }
    }

    return GetUIIDNone();
}

inline void RecyclableItemPool::ReturnItemToPool(const UIID & idItem)
{
    for (uint32 nIndex = 0; nIndex < m_arItemPool.size(); nIndex++)
    {
        RecyclableItem & tItem = m_arItemPool[nIndex];
        if (tItem.idItem == idItem)
        {
            UI_GetWindowFromID(tItem.idItem).SetActive(FALSE);
            tItem.bInUse = FALSE;
        }
    }
}

inline void RecyclableItemPool::ReturnMultipleItemsToPool(const UIIDArray & aridItems, int32 nStartingIndex, int32 nEndingIndex)
{
    for (int32 i = nStartingIndex; i <= nEndingIndex; i++)
    {
        ReturnItemToPool(aridItems[i]);
    }
}

inline const RecyclableItem * RecyclableItemPool::GetItemInfoByUIID(const UIID & idItem) const
{
    for (int32 nIndex = 0; nIndex < m_arItemPool.size(); nIndex++)
    {
        if (m_arItemPool[nIndex].idItem == idItem)
        {
            return &m_arItemPool[nIndex];
        }
    }

    return NULL;
}

inline CRefString RecyclableItemPool::GetItemID(const int32 nIndex)
{
    CRefString s;
    s.Format("item %d list", nIndex);
    return s;
}
