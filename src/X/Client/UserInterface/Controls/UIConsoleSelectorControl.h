// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleSelectorControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"
#include "../Console/ConsoleInputUtilities.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

struct BufferedInput;
struct UIControlDataExt;
struct Rect2D;
struct UIControlSourceKey;
class UIConsoleSelector;

class ConsoleSelectorDataSource {  // size 0x8
public:
    virtual int32 GetNumItems() const = 0;  // vtable slot 0
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const = 0;  // vtable slot 1
    virtual void InitializeItemUI(const UIConsoleSelector &, const UIID &, const int32) = 0;  // vtable slot 2
    virtual void UpdateItemUI(const UIConsoleSelector &, const UIID &, const int32) = 0;  // vtable slot 3
};
static_assert(sizeof(ConsoleSelectorDataSource) == 0x8, "ConsoleSelectorDataSource");

struct SelectorItem {  // size 0x10
    UIID idItem;  // 0x00
    uint64 nKey;  // 0x08
};
static_assert(sizeof(SelectorItem) == 0x10, "SelectorItem");

class ConsoleSelectorCreationParams : public WindowCreationParams {  // size 0x750
public:
    ConsoleSelectorCreationParams(const UIType, const UIID &, const UIID &);
    ConsoleSelectorCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIFunctionPtr pfnOnSelChange;  // 0x730
    BOOL bRequiresControllerFocus;  // 0x738
    BOOL bScrollWithRightStick;  // 0x73c
    BOOL bWrap;  // 0x740
    uint32 nStickSpeedLimitMS;  // 0x744
    SNO snoClickSound;  // 0x748
private:
    void sInit();
};
static_assert(sizeof(ConsoleSelectorCreationParams) == 0x750, "ConsoleSelectorCreationParams");

class UIConsoleSelector : public UIWindow {  // size 0x250
public:
    UIConsoleSelector(const ConsoleSelectorCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleSelector(const ConsoleSelectorCreationParams &, const Rect2D *);
    virtual ~UIConsoleSelector();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 106
        return eType == UIT_CONSOLESELECTOR || UIWindow::IsType(eType);
    }
    virtual BOOL CanAcceptControllerFocus() const { return m_bRequiresControllerFocus; }  // vtable slot 6; line 120
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13; line 131
    virtual int32 GetNumItems() const;  // vtable slot 34
    int32 GetSelectedIndex() const { return m_nSelectedIndex; }  // line 142
    void SelectNextItem(const BOOL fRun);
    void SelectPreviousItem(const BOOL fRun);
    void SetSelectedIndex(const int32 nIndex, const BOOL fRun);
    void ResetAllItems();
    void SetDataSource(ConsoleSelectorDataSource & tDataSource);
    const UIID & GetItemUIID() const;
    virtual void LostControllerFocus();  // vtable slot 7
    virtual void GainedControllerFocus();  // vtable slot 8
    void SetOnSelChangeFunction(const UIFunctionPtr pfnOnSelChange) {  // line 187
        m_pfnOnSelChange = pfnOnSelChange;
    }
    const UIFunctionPtr GetOnSelChangeFunction() const;
    void SetWrapping(BOOL bWrap);
    BOOL m_bRequiresControllerFocus;  // 0x1cc
    BOOL m_bScrollWithRightStick;  // 0x1d0
    BOOL m_bWrap;  // 0x1d4
    uint32 m_nInputSpeedLimitMS;  // 0x1d8
    int32 m_nSelectedIndex;  // 0x1dc
    SNO m_snoClickSound;  // 0x1e0
    UIID m_idArrowLeft;  // 0x1e8
    UIID m_idArrowRight;  // 0x1f0
    Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x1f8
    UIFunctionPtr m_pfnOnSelChange;  // 0x220
    ConsoleSelectorDataSource * m_ptDataSource;  // 0x228
    typedef blz::vector<SelectorItem, blz::allocator<SelectorItem> > SelectorItemArray;
    SelectorItemArray m_arSelectorItems;  // 0x230
    UIID m_idVisibleItem;  // 0x248
private:
    void sInit(const ConsoleSelectorCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim);
    BOOL sInput(const BufferedInput &);
    void CreateItem(const int32);
    BOOL HandleInput(const BufferedInput & tInput);
    BOOL HandleStickInput(const BufferedInput & tInput);
    BOOL HandleDPadInput(const BufferedInput & tInput);
    void ShowItem(int32 nItemIndex);
    void UpdateArrows();
};
static_assert(sizeof(UIConsoleSelector) == 0x250, "UIConsoleSelector");
