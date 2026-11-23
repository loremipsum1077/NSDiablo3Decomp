// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIDrawHookControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIControlControl.h"

struct UIControlDataExt;
struct Rect2D;

class DrawHookCreationParams : public ControlCreationParams {  // size 0xb0
public:
    DrawHookCreationParams(const UIType, const UIID &, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    virtual ~DrawHookCreationParams() {}  // vtable slot 0; line 18
    int32 m_nDrawHook;  // 0xa8
};
static_assert(sizeof(DrawHookCreationParams) == 0xb0, "DrawHookCreationParams");

class UIDrawHook : public UIControl {  // size 0x68
public:
    UIDrawHook(const DrawHookCreationParams &);
    virtual ~UIDrawHook() {}  // vtable slot 0; line 45
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 47
        return eType == UIT_DRAWHOOK || UIControl::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    int32 m_nDrawHook;  // 0x60
};
static_assert(sizeof(UIDrawHook) == 0x68, "UIDrawHook");
