// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIHotKeyControl.h

#pragma once

#include "UIControlControl.h"

struct BufferedInput;
struct UIControlDataExt;
struct Rect2D;

typedef BOOL (*UIHotkeyPredicate)(const UIID &);

// Key, KEY_* and the INPUTMSG_* values live in Input.h (DWARF: Input.h:174, :15,
// :42, :194..).  The typedef is kept here as well because Key is a redundant
// identical typedef and the DWARF reference header repeats it; Input.h itself is
// deliberately not included here, because UIControlControl.cpp defines its own
// stand-in BufferedInput/BufferedControllerData after including this header.
typedef int32 Key;

class HotkeyCreationParams : public ControlCreationParams {  // size 0xc8
public:
    HotkeyCreationParams(const UIType, const UIID &, const UIID &);
    HotkeyCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIFunctionPtr m_pfnOnKey;  // 0xa8
    UIConsoleFunctionPtr m_pfnOnController;  // 0xb0
    UIHotkeyPredicate m_pfnHotkeyPredicate;  // 0xb8
    int32 m_nUIAction;  // 0xc0
private:
    void sInit();
};
static_assert(sizeof(HotkeyCreationParams) == 0xc8, "HotkeyCreationParams");

class UIHotkey : public UIControl {  // size 0x88
public:
    UIHotkey(const HotkeyCreationParams &);
    virtual ~UIHotkey() {}  // vtable slot 0
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput();  // vtable slot 13
    int32 GetUIAction() const { return m_nUIAction; }  // line 66
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17
        return eType == UIT_HOTKEY || UIControl::IsType(eType);
    }
private:
    BOOL sHandleInput(const BufferedInput & tInputPassed);
    UIFunctionPtr m_pfnOnKey;  // 0x60
    UIConsoleFunctionPtr m_pfnOnController;  // 0x68
    UIHotkeyPredicate m_pfnHotkeyPredicate;  // 0x70
    int32 m_nUIAction;  // 0x78
    Key m_eLastInputKey;  // 0x7c
    uint32 m_dwTickLastProcessedKeyStillDown;  // 0x80
};
static_assert(sizeof(UIHotkey) == 0x88, "UIHotkey");
