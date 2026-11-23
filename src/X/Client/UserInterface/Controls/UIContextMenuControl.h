// Original file: X/Client/UserInterface/Controls/UIContextMenuControl.h
// No compile unit has this class's full definition: the DWARF (UI.cpp's unit) has only a declaration of
// UIContextMenu with SetCallerControlID (line 81) and GetCallerControlID (line 86), and no UIContextMenu function
// survives in the binary. Written for UI_Input (UI.cpp:3067-3069, sUpdateActivePopupControlVisibility), the one
// function in the binary that uses the class; the repo owner allowed inventing its layout for that function
// ("For this we can invent"). Only what that use needs is here; GetCallerControlID sits on its DWARF line.

#pragma once

#include "UIWindowControl.h"

// Base UIWindow (size 0x1d0): UI_OpenContextMenu (UI.cpp:766, linker-removed) names its UIContextMenu * local
// "ptWindow" (UI.cpp:822), and the static_cast from UIControl & at UI.cpp:3067 compiles to no pointer adjustment.
// The total size is unknown (at least 0x208), so only the caller id's offset is asserted.
class UIContextMenu : public UIWindow {
public:
































































    // line 81: SetCallerControlID(const UIID & callerControlId) is used only by linker-removed UI_OpenContextMenu; not written.




    const UIID & GetCallerControlID() const
    {
        return m_callerControlId;
    }

    // INVENTED: padding for the 0x30 bytes between the end of UIWindow (0x1d0) and the caller id (0x200); nothing in
    // the binary reads or writes them. Public, like the field below, only so the offsetof assert can name it.
    uint8 _unknown[0x200 - 0x1d0];  // 0x1d0
    // INVENTED: the name. The UIID at 0x200 is what GetCallerControlID returns (UI_Input reads it at +0x200, row
    // UIContextMenuControl.h:88:16); the parameter of SetCallerControlID (line 81) is named callerControlId.
    UIID m_callerControlId;  // 0x200
};
static_assert(offsetof(UIContextMenu, m_callerControlId) == 0x200, "UIContextMenu");
