// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIRadioButtonGroupControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIControlControl.h"
#include "UIControlGroup.h"
#include "../Console/ConsoleInputUtilities.h"

struct UIControlDataExt;
struct Rect2D;

class UIRadioButton;
struct BufferedInput;

class RadioButtonGroupCreationParams : public ControlCreationParams {  // size 0xb8; line 21
public:
    RadioButtonGroupCreationParams(const UIType, const UIID &, const UIID &);
    RadioButtonGroupCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIFunctionPtr pfnOnSelectionChanged;  // 0xa8
    uint32 nStickSpeedLimitMS;  // 0xb0
    int32 nControlStyle;  // 0xb4
private:
    void sInit();
};
static_assert(sizeof(RadioButtonGroupCreationParams) == 0xb8, "RadioButtonGroupCreationParams");

enum {
    RADIO_BUTTON_CONTROLLER_VERTICAL = 0,
    RADIO_BUTTON_CONTROLLER_HORIZONTAL = 1,
};

typedef int32 RadioButtonControllerInputStyle;

class UIRadioButtonGroup : public UIControlGroup {  // size 0xd0
public:
    UIRadioButtonGroup(const RadioButtonGroupCreationParams &);
    virtual ~UIRadioButtonGroup() {}  // vtable slot 0; line 67
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 69
        return eType == UIT_RADIOBUTTONGROUP || UIControl::IsType(eType);
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    void SetSelection(int32 nValue, const BOOL fForceSelChange);
    void SetSelection(const UIID & idControl);
    void SetShouldForceSelectionChange(BOOL);
    BOOL GetShouldForceSelectionChange() const;
    void SelectFirstEnabledButton();
    void SelectNextEnabledButton();
    void SelectPreviousEnabledButton();
    const UIID & GetSelection() const;
    int32 GetSelectedValue() const;
    UIRadioButton * GetButtonByValue(int32 nValue) const;
    void SetOnSelectionChangedCallback(UIFunctionPtr);
    static UIID Create(const RadioButtonGroupCreationParams &);
    void SetWrapSelection(BOOL bWrapSelection) { m_bWrapSelection = bWrapSelection; }  // line 139
protected:
    virtual void OnChildAdded(const UIID & idChild);  // vtable slot 27
    BOOL HandleInput(const BufferedInput & tInput);
    BOOL HandleStickInput(const BufferedInput & tInput);
    BOOL HandleDPadInput(const BufferedInput & tInput);
    RadioButtonControllerInputStyle m_eInputStyle;  // 0x88
    Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x90
    uint32 m_nInputSpeedLimitMS;  // 0xb8
    BOOL m_bWrapSelection;  // 0xbc
    UIFunctionPtr m_pfnOnSelectionChanged;  // 0xc0
    BOOL m_fShouldForceSelChange;  // 0xc8
};
static_assert(sizeof(UIRadioButtonGroup) == 0xd0, "UIRadioButtonGroup");
