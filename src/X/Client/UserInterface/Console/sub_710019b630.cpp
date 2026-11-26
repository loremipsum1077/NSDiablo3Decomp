// 2.7.7: Kanai's powers panel, an unnamed 0x30-byte class (0x710019aec0..0x710019bb60) shared with the skills,
// leaderboard-inspect and equipment-manager skill screens. Only the scroll helpers are written here; the constructor
// sub_710019b630, the child constructor sub_710019aec0, the child refresh sub_710019b0b0 and the panel refresh
// sub_710019bab0 are not decompiled yet.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "UIConsoleUtils.h"

UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils

// One of the four 0xd8-byte children (one per Kanai's cube slot); unk_18 is its scroll box.
struct Unk19aec0 {
    uint8 unk_00[0x18];
    UIID unk_18;  // 0x18
    uint8 unk_20[0xb8];
};
static_assert(sizeof(Unk19aec0) == 0xd8, "Unk19aec0");

struct Unk19b630 {  // size 0x30
    Unk19aec0 * unk_00[4];  // 0x00
    uint64 unk_20;  // 0x20
    UIID unk_28;  // 0x28
};
static_assert(sizeof(Unk19b630) == 0x30, "Unk19b630");

// Probably: Kanai's powers panel, TRUE if any of the four child scroll boxes can scroll.
extern "C" BOOL sub_710019b960(Unk19b630 * pThis)
{
    for (int32 i = 0; i < 4; i++)
    {
        if (UIUtils::Console::CanScrollContentInScrollBox(pThis->unk_00[i]->unk_18))
            return TRUE;
    }
    return FALSE;
}

// Probably: Kanai's powers panel, scrolls the four child scroll boxes back to the top.
extern "C" void sub_710019b9d0(Unk19b630 * pThis)
{
    UI_GetConsoleScrollBoxFromID(pThis->unk_00[0]->unk_18).SetScrollPos(0.0f);
    UI_GetConsoleScrollBoxFromID(pThis->unk_00[1]->unk_18).SetScrollPos(0.0f);
    UI_GetConsoleScrollBoxFromID(pThis->unk_00[2]->unk_18).SetScrollPos(0.0f);
    UI_GetConsoleScrollBoxFromID(pThis->unk_00[3]->unk_18).SetScrollPos(0.0f);
}

// Probably: Kanai's powers panel, scrolls the four child scroll boxes by flDelta.
extern "C" void sub_710019ba50(Unk19b630 * pThis, float flDelta)
{
    UIUtils::Console::ScrollContentInScrollBox(pThis->unk_00[0]->unk_18, flDelta);
    UIUtils::Console::ScrollContentInScrollBox(pThis->unk_00[1]->unk_18, flDelta);
    UIUtils::Console::ScrollContentInScrollBox(pThis->unk_00[2]->unk_18, flDelta);
    UIUtils::Console::ScrollContentInScrollBox(pThis->unk_00[3]->unk_18, flDelta);
}
