// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIKeyMappingButtonControl.h
// `m_eLastKey` is the DWARF's `Key` (a typedef for int32 in X/Client/Input/Input.h);
// using int32 keeps this header independent of Input.h, which some TUs include after
// forward-declaring `BufferedInput` themselves.

#pragma once

#include "UIButtonControl.h"
#include "../UIMessage.h"
#include "../UITypes.h"

struct UIControlDataExt;
struct Rect2D;

class KeyMappingButtonCreationParams : public ButtonCreationParams {  // size 0x898
public:
    KeyMappingButtonCreationParams(const UIType, const UIID &, const UIID &);
    KeyMappingButtonCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIFunctionPtr pfnOnKey;  // 0x890
private:
    void sInit();
};
static_assert(sizeof(KeyMappingButtonCreationParams) == 0x898, "KeyMappingButtonCreationParams");

class UIKeyMappingButton : public UIButton {  // size 0x4d8
public:
    UIKeyMappingButton(const KeyMappingButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIKeyMappingButton(const KeyMappingButtonCreationParams &, const Rect2D *);
    virtual ~UIKeyMappingButton() {}  // vtable slot 0; line 52
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 58
        return eType == UIT_KEYMAPPINGBUTTON || UIButton::IsType(eType);
    }
private:
    void sInit(const KeyMappingButtonCreationParams & tParams);
    int32 m_eLastKey;  // 0x4c8; DWARF type `Key`, which is `typedef int32`
    UIFunctionPtr m_pfnOnKey;  // 0x4d0
};
static_assert(sizeof(UIKeyMappingButton) == 0x4d8, "UIKeyMappingButton");
