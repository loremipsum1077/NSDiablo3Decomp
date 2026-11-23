// Written by hand: the DWARF has no reference header for X/Client/UserInterface/Interface/UIConfirmation.h
// because it declares no types. The functions and their order follow UIConfirmation.cpp; the
// mangled names (_ZN14UIConfirmation...) make UIConfirmation a namespace.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIFunctions.h"
#include "../../../Sigma/SigmaTypes.h"

struct UIID;

namespace UIConfirmation {
void OnOK(const UIID & id);
void OnCancel(const UIID & id);
void Reset();
BOOL IsVisible();
BOOL IsOpenWithFunction(UIFunctionPtr pfnOnClickOK);
void Close();
void Show(LPCSTR szMessage, UIFunctionPtr pfnOnClickOK, UIFunctionPtr pfnOnClickCancel, BOOL bCloseOnMovement, BOOL bRestoreCursorStateOnOK, BOOL bUseHotkeys, int32 nControllerIndex);
void PlayerMoved(int32 nControllerIndex);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
}  // namespace UIConfirmation
