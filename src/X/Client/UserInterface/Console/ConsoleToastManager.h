// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/ConsoleToastManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ToastManager.h"
#include "../../../Sigma/Unicode.h"

struct UIID;

enum {
    CONSOLE_TOAST_DURATION_SECONDS = 4,
};

class ConsoleToastManager : public ToastManagerBase {  // size 0x1d8
public:
    ConsoleToastManager();
    void SendToastConsole(LPCSTR szMessage, const uint32 dwToastDurationTicks);
    ToastId SendIndefiniteToastConsole(LPCSTR szMessage);
protected:
    virtual const UIID CreateToastVisual(int32 nToastSlot, ToastManagerBase::Toast * ptToast);  // vtable slot 4
    virtual void DisposeToastVisual(ToastManagerBase::Toast * ptToast);  // vtable slot 5
    virtual void UpdateToastVisual(int32, ToastManagerBase::Toast * ptToast, float flDeltaTime);  // vtable slot 6
    virtual void OutroToastVisual(ToastManagerBase::Toast * ptToast, float flDeltaTime);  // vtable slot 7
};
static_assert(sizeof(ConsoleToastManager) == 0x1d8, "ConsoleToastManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  ConsoleToastManager::~ConsoleToastManager()  [0x7100109ff0]
