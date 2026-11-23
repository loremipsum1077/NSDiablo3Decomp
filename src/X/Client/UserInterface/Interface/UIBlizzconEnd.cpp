// `X/Client/UserInterface/Interface/UIBlizzconEnd.cpp` in the 2.6.2 build.
#include <stddef.h>

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"
#include "../../CGame.h"
#include "../../CBlizzconTimer.h"
#include "../UIGlobals.h"
#include "UIBlizzconEnd.h"

// UIBlizzconEnd.cpp:33 (UIBlizzconGlobals, 0x10 bytes). The module's whole state. It is at
// global scope, not inside the namespace: `Initialize` mangles `_ZN17UIBlizzconGlobals10InitializeEv`.
struct UIBlizzconGlobals {  // size 0x10
    void Initialize() {
        idStoryboardToShow = GetUIIDNone();
        uTickToCloseUI = 0;
    }

    UIID idStoryboardToShow;  // 0x00
    uint32 uTickToCloseUI;  // 0x08
};
static_assert(sizeof(UIBlizzconGlobals) == 0x10, "UIBlizzconGlobals");
static_assert(offsetof(UIBlizzconGlobals, idStoryboardToShow) == 0x0, "UIBlizzconGlobals::idStoryboardToShow");
static_assert(offsetof(UIBlizzconGlobals, uTickToCloseUI) == 0x8, "UIBlizzconGlobals::uTickToCloseUI");

// UIBlizzconEnd.cpp:49.
static UIBlizzconGlobals * sgptUIBlizzconGlobals;

// UIBlizzconEnd.cpp:51 -- "the player dismissed the screen". Declared `BOOL`, but it only ever
// holds `FALSE`/`TRUE`, so GlobalOpt shrinks it and every read is `ldrb` plus a bit test.
static BOOL bSkipped;

// UIBlizzconEnd.cpp:53.
static const uint32 TICKS_UNTIL_SCREEN_CLOSE = 2700;

namespace UIBlizzconEnd {

// UIBlizzconEnd.cpp:60.
void InitializeForGame() {
    sgptUIBlizzconGlobals = new UIBlizzconGlobals;
    sgptUIBlizzconGlobals->Initialize();
}

// UIBlizzconEnd.cpp:69.
void DisposeForGame() {
    delete sgptUIBlizzconGlobals;
    sgptUIBlizzconGlobals = NULL;
}

// UIBlizzconEnd.cpp:78 -- put the end screen up over the console death menu. `nScreenToShow` 0 is
// the timed screen, which arms the auto-close stamp.
void Show(int32 nScreenToShow) {
    if (!sgptUIBlizzconGlobals) {
        return;
    }

    if (IsActive()) {
        return;
    }

    UI_GetWindowFromID(UIDLG_CONSOLE_DEATHMENU).SetActive(FALSE);

    UI_GetWindowFromID(UIWND_VICTORYSCREEN_BLIZZCON_2014).SetActive(TRUE);

    bSkipped = FALSE;

    if (nScreenToShow == 0) {
        sgptUIBlizzconGlobals->uTickToCloseUI = CGameGetClientTicks() + TICKS_UNTIL_SCREEN_CLOSE;
    }
}

// UIBlizzconEnd.cpp:133 -- inlined into both `Show` and `Update`.
BOOL IsActive() {
    if (!UI_TestID(UIWND_VICTORYSCREEN_BLIZZCON_2014)) {
        return FALSE;
    }

    if (!sgptUIBlizzconGlobals) {
        return FALSE;
    }

    return UI_GetWindowFromID(UIWND_VICTORYSCREEN_BLIZZCON_2014).IsActive();
}

// UIBlizzconEnd.cpp:149 -- the per-frame tick: unpause, then close the screen once the stamp
// expires or the player skipped it.
void Update() {
    if (!IsActive()) {
        return;
    }

    if (!sgptUIBlizzconGlobals) {
        return;
    }

    if (GameIsPaused()) {
        GamePause(FALSE);
    }

    if (sgptUIBlizzconGlobals->uTickToCloseUI != 0 && sgptUIBlizzconGlobals->uTickToCloseUI <= CGameGetClientTicks() || bSkipped) {
        bSkipped = FALSE;
        sgptUIBlizzconGlobals->uTickToCloseUI = 0;

        CBlizzconReturnToStartScreen();
    }
}

// UIBlizzconEnd.cpp:179 -- dropped by `--gc-sections` from the shipped build. Its DWARF DIE
// (0x0381e922, `_ZN13UIBlizzconEnd11CheatClosedEv`, low_pc 0x0, high_pc 0x10, decl_line 179,
// external) shows the 16-byte body `adrp; mov #1; strb; ret`. It still has to exist: it is the
// only writer of `TRUE` into `bSkipped`, and without it IPSCCP proves the flag constant `FALSE`
// and deletes every read of it out of `Update`.
void CheatClosed() {
    bSkipped = TRUE;
}

}  // namespace UIBlizzconEnd
