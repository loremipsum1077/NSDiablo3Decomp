// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleEditControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIEditControl.h"

struct UIControlDataExt;
struct Rect2D;

class UIConsoleEdit : public UIEdit {  // size 0x470
public:
    UIConsoleEdit(const EditBoxCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleEdit(const EditBoxCreationParams & tParams, const Rect2D * pRect) : UIEdit(tParams, pRect) {}  // line 29
    virtual ~UIConsoleEdit() {}  // vtable slot 0, line 36
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13, line 43
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17, line 50
        return eType == UIT_CONSOLEEDIT || UIEdit::IsType(eType);
    }
};
static_assert(sizeof(UIConsoleEdit) == 0x470, "UIConsoleEdit");

// UIConsoleEditControl.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
namespace UIConsole {
void InitializeForApp();
void DisposeForApp();
}  // namespace UIConsole
