// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleListboxControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Console/ConsoleInputUtilities.h"
#include "UIControlGroup.h"
#include "UIScrollbarControl.h"
#include "UIWindowControl.h"
#include "../Interface/UIFunctions.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct BufferedInput;
class CRefString;
struct Rect2D;
struct RecyclableItem;
struct UIControlDataExt;
struct UIControlSourceKey;
struct Vector2D;
class UIConsoleListBox;

class ConsoleListBoxDataSource {  // size 0x8
public:
    virtual ~ConsoleListBoxDataSource() {}  // vtable slot 0; line 28
    virtual int32 GetNumItems() const = 0;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const = 0;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32) = 0;  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32) = 0;  // vtable slot 5
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex) {  // vtable slot 6; line 54
        return TRUE;
    }
};
static_assert(sizeof(ConsoleListBoxDataSource) == 0x8, "ConsoleListBoxDataSource");

enum {
    CONSOLE_LISTBOX_STYLE_LIST_VERTICAL = 0,
    CONSOLE_LISTBOX_STYLE_GRID = 1,
    CONSOLE_LISTBOX_STYLE_LIST_HORIZONTAL = 2,
};

typedef int32 ConsoleListBoxStyle;

class ConsoleListBoxCreationParams : public WindowCreationParams {  // size 0x768
public:
    ConsoleListBoxCreationParams(const UIType, const UIID &, const UIID &);
    ConsoleListBoxCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIListBoxChangeFunctionPtr pfnOnSelChange;  // 0x730
    BOOL bRequiresControllerFocus;  // 0x738
    BOOL bOnlyScrollWithDPad;  // 0x73c
    BOOL bScrollWithRightStick;  // 0x740
    uint32 nStickSpeedLimitMS;  // 0x744
    int32 nStackSpacing;  // 0x748
    ScrollbarVisibility eScrollbarVisibility;  // 0x74c
    float flScrollIndicatorPaddingTop;  // 0x750
    float flScrollIndicatorPaddingRight;  // 0x754
    float flScrollIndicatorPaddingBottom;  // 0x758
    SNO snoMouseoverSound;  // 0x75c
    ConsoleListBoxStyle eStyle;  // 0x760
    BOOL bSupportsTouch;  // 0x764
private:
    void sInit();
};
static_assert(sizeof(ConsoleListBoxCreationParams) == 0x768, "ConsoleListBoxCreationParams");

class RecyclableItemPool {  // size 0x18
public:
    ~RecyclableItemPool();
    UIID AddItemToPool(const UIControlSourceKey & tItemTemplate, const UIID & idParent);
    UIID ReserveItemFromPool(uint64 nKey, const UIID & idParent);
    void ReturnItemToPool(const UIID & idItem);
    void ReturnMultipleItemsToPool(const UIIDArray & aridItems, int32 nStartingIndex, int32 nEndingIndex);
    const RecyclableItem * GetItemInfoByUIID(const UIID & idItem) const;
    typedef blz::vector<RecyclableItem, blz::allocator<RecyclableItem> > RecyclableItemArray;
private:
    RecyclableItemPool::RecyclableItemArray m_arItemPool;  // 0x00
    static CRefString GetItemID(const int32 nIndex);
};
static_assert(sizeof(RecyclableItemPool) == 0x18, "RecyclableItemPool");

class UIConsoleListBox : public UIWindow {  // size 0x2b0
public:
    UIConsoleListBox(const ConsoleListBoxCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleListBox(const ConsoleListBoxCreationParams &, const Rect2D *);
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 178
        if (eType == UIT_CONSOLELISTBOX)
        {
            return TRUE;
        }

        return UIWindow::IsType(eType);
    }
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    virtual BOOL CanAcceptControllerFocus() const { return m_bRequiresControllerFocus; }  // vtable slot 6; line 197
    int32 GetNumItems() const;
    int32 GetNumRows() const;
    int32 GetNumItemsPerRow() const;
    int32 GetNumVisibleItems() const;
    int32 GetNumVisibleRows() const;
    int32 GetRowForIndex(const int32 nIndex) const;
    int32 GetSelectedIndex() const { return m_nSelectedIndex; }  // line 232
    void SetSelectedIndex(const int32 nIndex);
    int32 GetFirstVisibleItemIndex() const {  // line 244
        return m_nFirstVisibleRow == -1 ? -1 : m_nFirstVisibleRow * m_nNumItemsPerRow;
    }
    int32 GetLastVisibleItemIndex() const {  // line 251
        return Math::Min(GetFirstVisibleItemIndex() + GetNumVisibleItems(),
                         GetNumItems()) - 1;
    }
    void SetFirstVisibleItemIndex(const int32 nIndex);
    void SetFirstVisibleRow(const int32 nRow);
    void SetLastVisibleItemIndex(const int32);
    void SetLastVisibleRow(const int32 nRow);
    BOOL IsItemVisible(const int32 nIndex) const;
    void ScrollItemIntoView(const int32 nItemIndex);
    void ResetAllItems();
    void RefreshItem(const int32 nIndex);
    void RefreshVisibleItems();
    void SetDataSource(ConsoleListBoxDataSource & tDataSource);
    void ClearDataSource();
    void SetOnSelChangeFunction(UIListBoxChangeFunctionPtr pfnOnSelChange) {  // line 313
        m_pfnOnSelChange = pfnOnSelChange;
    }
    void ForceCallToSelectionChange() {  // line 319
        if (m_pfnOnSelChange != NULL)
        {
            m_pfnOnSelChange(GetID(), m_nSelectedIndex, m_nSelectedIndex);
        }
    }
    void SetOnScrolledOffTopFunction(UIFunctionPtr pfnOnScrolledOffTop) {  // line 328
        m_pfnOnScrolledOffTop = pfnOnScrolledOffTop;
    }
    void SetOnScrolledOffBottomFunction(UIFunctionPtr pfnOnScrolledOffBottom) {  // line 334
        m_pfnOnScrolledOffBottom = pfnOnScrolledOffBottom;
    }
    void SimulateStickInput(BOOL bStickUp);
    float GetScrollIndicatorPaddingTop() const;
    float GetScrollIndicatorPaddingRight() const;
    float GetScrollIndicatorPaddingBottom() const;
    void SetScrollIndicatorPadding(const float, const float, const float);
    const UIID & GetItemUIID(const int32 nIndex) const;
    void SelectNextItem(BOOL bPlaySound);
    void SelectPreviousItem(BOOL bPlaySound);
    void SelectItemBelow(BOOL bPlaySound);
    void SelectItemAbove(BOOL bPlaySound);
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13; line 393
    virtual void LostControllerFocus();  // vtable slot 7
    virtual void GainedControllerFocus();  // vtable slot 8
    int32 GetNextSelectableIndex() const;
    int32 GetPrevSelectableIndex() const;
    void SetAnalogStickInputFilter(const Console::AnalogStickInputFilter & tStickInputFilter) {  // line 411
        m_tStickInputFilter = tStickInputFilter;
        m_tStickInputFilter.SetMinimumInputDelay(m_nInputSpeedLimitMS);
    }
    void SetAnalogStickMinimumInputDelay(uint32 nInputSpeedLimitMS);
    Blizzard::Time::Millisecond GetAnalogStickMinimumInputDelay();
    void ResetAnalogMinimumInputDelay();
    int32 GetScrollDirection() const { return m_nDirection; }  // line 427
    UIID GetScrollbarIndicator() { return m_idScrollIndicator; }  // line 433
protected:
    virtual BOOL ForwardMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 28
private:
    UIID m_idStackPanel;  // 0x1d0
    UIID m_idScrollIndicator;  // 0x1d8
    int32 m_nSelectedIndex;  // 0x1e0
    int32 m_nFirstVisibleRow;  // 0x1e4
    int32 m_nMaxVisibleItems;  // 0x1e8
    BOOL m_bOnlyScrollWithDPad;  // 0x1ec
    BOOL m_bScrollWithRightStick;  // 0x1f0
    BOOL m_bRequiresControllerFocus;  // 0x1f4
    uint32 m_nInputSpeedLimitMS;  // 0x1f8
    float m_flScrollIndicatorPaddingTop;  // 0x1fc
    float m_flScrollIndicatorPaddingRight;  // 0x200
    float m_flScrollIndicatorPaddingBottom;  // 0x204
    SNO m_snoMouseoverSound;  // 0x208
    UIIDArray m_aridVisibleItems;  // 0x210
    UIIDArray m_aridRowStackPanels;  // 0x228
    UIListBoxChangeFunctionPtr m_pfnOnSelChange;  // 0x240
    UIFunctionPtr m_pfnOnScrolledOffTop;  // 0x248
    UIFunctionPtr m_pfnOnScrolledOffBottom;  // 0x250
    ConsoleListBoxDataSource * m_ptDataSource;  // 0x258
    RecyclableItemPool m_tItemPool;  // 0x260
    Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x278
    ConsoleListBoxStyle m_eStyle;  // 0x2a0
    int32 m_nNumItemsPerRow;  // 0x2a4
    int32 m_nDirection;  // 0x2a8
    BOOL m_bSupportsTouch;  // 0x2ac
    void sInit(const ConsoleListBoxCreationParams & tParams, const Vector2D &, const Vector2D &);
    int32 GetSelectableIndexAbove() const;
    int32 GetSelectableIndexBelow() const;
    void CalculateMaxNumberOfVisibleItems();
    void GetVisibleIndexRange(const int32, const int32, int32 &, int32 &);
    void HandleItemsInserted(const ConsoleListBoxDataSource *, const int32, const int32);
    void HandleItemsRemoved(const ConsoleListBoxDataSource *, const int32, const int32);
    void CreateItem(const int32 nItemIndex);
    void CreateMultipleItems(const int32 nStartingIndex, const int32 nEndingIndex);
    void RemoveMultipleItems(const int32 nStartingIndex, const int32 nEndingIndex);
    BOOL HandleInput(const BufferedInput & tInput);
    BOOL HandleStickInput(const BufferedInput & tInput);
    BOOL HandleDPadInput(const BufferedInput & tInput);
    void OnUpdate();
    void HandleTouch();
    void HandleTouchStopped();
    void PositionChildControls();
    void PlaySelectionSound();
    UIID CreateGridRowStackPanel(const int32 nRow);
    UIID GetGridRowStackPanelId(const int32 nRow);
};
static_assert(sizeof(UIConsoleListBox) == 0x2b0, "UIConsoleListBox");
