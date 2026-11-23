// Original file: X/Client/UserInterface/Console/UIConsoleFirstTimeFlow.cpp
//
// The guided first-run sequence the start screen hands off to: the safe-zone
// rectangle, then gamma calibration, then (on platforms that have it) the
// Battle.net link offer, after which the preferences flag is set and saved.
// `UIFirstTimeFlow::Console` is a namespace of free functions and the flow's
// whole state is the heap-backed `ConsoleFirstTimeFlowData` behind the
// TU-local pointer below. `sShowGammaAdjust` is file-static (mangled
// `_ZN15UIFirstTimeFlow7ConsoleL16sShowGammaAdjustEv`) and carries no symbol of
// its own; `Close` and `OnBattlenetLinkComplete` have external linkage
// (DW_AT_external in the CU), were inlined at every call site in this build, and
// survive in the DWARF only as low_pc-0 linker-stripped out-of-line copies (the
// same convention described in UIConfirmation.cpp).

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../Interface/UIFunctions.h"
#include "../UIID.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "ControllerActions.h"

// Not used directly: the original reaches these through an include chain this file does not
// reproduce; they are here for the globals they add to the static initializer (SNONAME_NONE,
// cgplaceNull, blz::ignore, the GfxVB.h normals and the nn::ldn addresses).
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/Console/UIConsoleSafeZoneSettings.cpp
namespace UISafeZoneSettings {
BOOL IsVisible();
void Close();
void Open(const int32 nControllerIndex);
}  // namespace UISafeZoneSettings
// X/Client/UserInterface/Console/UIConsoleGammaAdjust.cpp
namespace UIGammaAdjust {
namespace Console {
BOOL IsVisible();
void Close();
void SetOnCloseFunction(const UIFunctionPtr pfnOnClose);
void OpenForController(int32 nControllerIndex, BOOL bInFirstTimeFlow);
}  // namespace Console
}  // namespace UIGammaAdjust
// X/Client/UserInterface/Console/UIConsoleBattlenetLink.cpp
namespace UIBattlenetLink {
BOOL IsVisible();
void Close();
}  // namespace UIBattlenetLink
// X/Client/UserInterface/UI.cpp
void UI_SetPrimaryUIController(int32 nPrimaryController);
// X/Client/Console/GamerProfileNX64.cpp
namespace Console {
namespace GamerProfile {
void SavePrefs();
}  // namespace GamerProfile
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Screens {
ControllerActionManager & GetActionManager();
void UpdateButtonLegendText();
}  // namespace Screens
}  // namespace Console

namespace UIFirstTimeFlow {
namespace Console {

// :45 -- the flow's whole state: whether it is running and which controller slot started it.
// A constructor that assigns its members keeps both fields the one `str xzr` that
// `InitializeForGame` carries.
struct ConsoleFirstTimeFlowData {  // size 0x8
    ConsoleFirstTimeFlowData()
        : bActive(FALSE),
          nControllerIndex(0) {}

    BOOL bActive;  // 0x00
    int32 nControllerIndex;  // 0x04
};
static_assert(sizeof(ConsoleFirstTimeFlowData) == 0x8, "ConsoleFirstTimeFlowData");

// :50 -- the flow's state, internal linkage (readers are plain `adrp`/`ldr`, no GOT indirection).
static ConsoleFirstTimeFlowData * sgptConsoleFirstTimeFlowData;

// Declared (in the original, in the file's header) before their first use, which precedes their
// definitions in this file.
void OnGammaAdjustComplete(const UIID &);  // :107
void Close();  // :65
void OnBattlenetLinkComplete(const UIID &);  // :127
BOOL IsActive();  // :170

// :52
void InitializeForGame()
{
    sgptConsoleFirstTimeFlowData = new ConsoleFirstTimeFlowData();
}

// :58
void DisposeForGame()
{
    delete sgptConsoleFirstTimeFlowData;
    sgptConsoleFirstTimeFlowData = NULL;
}

// :65 -- leaving the flow is a single flag clear; the record itself lives until DisposeForGame.
void Close()
{
    sgptConsoleFirstTimeFlowData->bActive = FALSE;
}

// :71 -- back out of whichever step is on screen, then drop the flag.
void Cancel()
{
    if (UISafeZoneSettings::IsVisible()) {
        UISafeZoneSettings::Close();
    }

    if (UIGammaAdjust::Console::IsVisible()) {
        UIGammaAdjust::Console::Close();
    }

    if (UIBattlenetLink::IsVisible()) {
        UIBattlenetLink::Close();
    }

    Close();
}

// :92 -- step two: hand the gamma screen this flow's completion hook and open it for the slot
// that started the flow.
static void sShowGammaAdjust()
{
    UIGammaAdjust::Console::SetOnCloseFunction(OnGammaAdjustComplete);
    UIGammaAdjust::Console::OpenForController(sgptConsoleFirstTimeFlowData->nControllerIndex, TRUE);
}

// :99
void OnSafeZoneSetupComplete()
{
    UISafeZoneSettings::Close();

    sShowGammaAdjust();
}

// :107 -- step three. The Battle.net link step is skipped on this platform, so OnBattlenetLinkComplete
// is inlined straight in here and the whole tail (the second IsActive test included) is part of
// this body.
void OnGammaAdjustComplete(const UIID &)
{
    if (IsActive()) {
        Prefs::GetFirstTimeFlowCompleted() = TRUE;
        ::Console::GamerProfile::SavePrefs();

        // The DWARF leaves this function's parameter unnamed and the inlined callee's argument
        // emitted no code, so a default-constructed placeholder stands in for it (the name is
        // invented; see CONTRIBUTING.md's names rule).
        OnBattlenetLinkComplete(UIID());
    }
}

// :127 -- the flow's last step: clear the button legend the steps installed and leave.
void OnBattlenetLinkComplete(const UIID &)
{
    if (IsActive()) {
        ::Console::Screens::GetActionManager().ClearAllActions();
        ::Console::Screens::UpdateButtonLegendText();

        Close();
    }
}

// :140 -- step one, and the flow's entry point. Re-entry while the flow is already up is a no-op.
void PerformFirstTimeFlow(int32 nControllerIndex)
{
    if (sgptConsoleFirstTimeFlowData->bActive) {
        return;
    }

    sgptConsoleFirstTimeFlowData->bActive = TRUE;
    sgptConsoleFirstTimeFlowData->nControllerIndex = nControllerIndex;

    UI_SetPrimaryUIController(nControllerIndex);

    UISafeZoneSettings::Open(sgptConsoleFirstTimeFlowData->nControllerIndex);
}

// :170
BOOL IsActive()
{
    return sgptConsoleFirstTimeFlowData && sgptConsoleFirstTimeFlowData->bActive;
}

}  // namespace Console
}  // namespace UIFirstTimeFlow
