// 2.7.7: Kanai's powers panel, an unnamed 0x30-byte class (0x710019aec0..0x710019bb60) shared with the skills,
// leaderboard-inspect and equipment-manager skill screens. Types are in sub_710019b630.h.
#include <new>
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../UIID.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIEquipLegendaryPowersCommon.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/RefString.h"
#include "UIConsoleUtils.h"
#include "sub_710019b630.h"

UIWindow & UI_GetWindowFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
UIText & UI_GetTextFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
CRefString StringListFetch(LPCSTR szLabel);
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString GBIDToString(const GBID & gbid);
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils
extern "C" int32 sub_710024d7e0();  // UITransmuteCommon.cpp, fourth Kanai's cube slot unlocked

// NON_MATCHING: instruction scheduling of the compact-layout stores at the end (same stores and values)
// Probably: Kanai's powers panel slot constructor. Stores the slot's five control UIIDs, captures their layout
// positions, and builds the compact layout (slot origin at flX, flY) used when the fourth slot is unlocked.
extern "C" void sub_710019aec0(Unk19aec0 * pThis, int32 nSlot, const UIID & idWindow, const UIID & idVisual,
                               const UIID & idScrollBox, const UIID & idName, const UIID & idDetails, float flX,
                               float flY, bool bFourthSlotOnly)
{
    pThis->unk_00 = nSlot;
    new (&pThis->unk_08) UIID(idWindow);
    new (&pThis->unk_10) UIID(idVisual);
    new (&pThis->unk_18) UIID(idScrollBox);
    new (&pThis->unk_20) UIID(idName);
    new (&pThis->unk_28) UIID(idDetails);
    pThis->unk_30 = bFourthSlotOnly;
    UI_GetWindowFromID(pThis->unk_08).GetRelativePosition(pThis->unk_34, pThis->unk_3c);
    UI_GetWindowFromID(pThis->unk_10).GetRelativePosition(pThis->unk_44, pThis->unk_4c);
    UI_GetConsoleScrollBoxFromID(pThis->unk_18).GetRelativePosition(pThis->unk_54, pThis->unk_5c);
    UI_GetTextFromID(pThis->unk_20).GetRelativePosition(pThis->unk_64, pThis->unk_6c);
    UI_GetTextFromID(pThis->unk_28).GetRelativePosition(pThis->unk_74, pThis->unk_7c);
    pThis->unk_94.y = pThis->unk_44.y;
    pThis->unk_9c.x = pThis->unk_4c.x;
    pThis->unk_9c.y = pThis->unk_4c.y;
    pThis->unk_a4.x = pThis->unk_54.x;
    pThis->unk_a4.y = pThis->unk_54.y;
    pThis->unk_b4.x = pThis->unk_64.x;
    pThis->unk_b4.y = pThis->unk_64.y;
    pThis->unk_c4.x = pThis->unk_74.x;
    pThis->unk_c4.y = pThis->unk_74.y;
    pThis->unk_ac.x = 242.0f;
    pThis->unk_ac.y = 134.0f;
    pThis->unk_bc.x = 242.0f;
    pThis->unk_bc.y = 29.0f;
    pThis->unk_cc.x = 242.0f;
    pThis->unk_cc.y = 40.0f;
    pThis->unk_94.x = 102.0f;
    pThis->unk_8c.x = 242.0f;
    pThis->unk_8c.y = 214.0f;
    pThis->unk_84.x = flX;
    pThis->unk_84.y = flY;
}

// Probably: Kanai's powers panel slot refresh. Shows the slot's power name, description and icon, and applies the
// compact or the normal layout; a fourth-slot-only child is hidden while the fourth slot is locked.
extern "C" void sub_710019b0b0(Unk19aec0 * pThis, int32 gbidPower, BOOL bFourthSlotUnlocked, int32 nClass, BOOL bIsMale)
{
    if (pThis->unk_30 && !bFourthSlotUnlocked)
    {
        UI_GetWindowFromID(pThis->unk_08).SetActive(FALSE);
        return;
    }

    blz::string sPowerName;
    blz::string sPowerDesc;
    if (gbidPower != -1)
    {
        sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
        sPowerDesc = UIEquipLegendaryPowers::GetPowerTooltipText(gbidPower, false);
    }
    else
    {
        sPowerName = StringListFetch("ConsoleSkills:NoKanaiPowerAssigned");
    }

    if (nClass != -1)
        UIEquipLegendaryPowers::ShowPowerIcon(pThis->unk_10, gbidPower, nClass, bIsMale);
    else
        UIEquipLegendaryPowers::ShowPowerIcon(pThis->unk_10, gbidPower, TRUE);
    UI_GetTextFromID(pThis->unk_20).SetText(sPowerName.c_str(), FALSE);
    UI_GetTextFromID(pThis->unk_28).SetText(sPowerDesc.c_str(), FALSE);

    if (bFourthSlotUnlocked)
    {
        UI_GetWindowFromID(pThis->unk_08).SetRelativePosition(pThis->unk_84, pThis->unk_8c);
        UI_GetWindowFromID(pThis->unk_10).SetRelativePosition(pThis->unk_94, pThis->unk_9c);
        UI_GetConsoleScrollBoxFromID(pThis->unk_18).SetRelativePosition(pThis->unk_a4, pThis->unk_ac);
        UI_GetTextFromID(pThis->unk_20).SetRelativePosition(pThis->unk_b4, pThis->unk_bc);
        UI_GetTextFromID(pThis->unk_28).SetRelativePosition(pThis->unk_c4, pThis->unk_cc);
    }
    else
    {
        UI_GetWindowFromID(pThis->unk_08).SetRelativePosition(pThis->unk_34, pThis->unk_3c);
        UI_GetWindowFromID(pThis->unk_10).SetRelativePosition(pThis->unk_44, pThis->unk_4c);
        UI_GetConsoleScrollBoxFromID(pThis->unk_18).SetRelativePosition(pThis->unk_54, pThis->unk_5c);
        UI_GetTextFromID(pThis->unk_20).SetRelativePosition(pThis->unk_64, pThis->unk_6c);
        UI_GetTextFromID(pThis->unk_28).SetRelativePosition(pThis->unk_74, pThis->unk_7c);
    }
}

// Probably: Kanai's powers panel constructor (parent window idParent): one child per cube slot, the universal
// (fourth) slot only shown when it is unlocked. Each child is owned (old pointer replaced, then deleted).
extern "C" void sub_710019b630(Unk19b630 * pThis, const UIID & idParent)
{
    pThis->unk_00[0] = NULL;
    pThis->unk_00[1] = NULL;
    pThis->unk_00[2] = NULL;
    pThis->unk_00[3] = NULL;
    new (&pThis->unk_20) UIID(idParent);
    new (&pThis->unk_28) UIID(UIIDCreateFromParent(pThis->unk_20, ".2x2Background"));

    Unk19aec0 * pChild;
    Unk19aec0 * pOld;
    {
        UIID idWindow = UIIDCreateFromParent(pThis->unk_20, ".WeaponPower");
        UIID idVisual = UIIDCreateFromParent(pThis->unk_20, ".WeaponPower.ItemVisual");
        UIID idScrollBox = UIIDCreateFromParent(pThis->unk_20, ".WeaponPower.ScrollableInfo");
        UIID idName = UIIDCreateFromParent(pThis->unk_20, ".WeaponPower.ScrollableInfo.WeaponDetailsStack.WeaponPowerName");
        UIID idDetails = UIIDCreateFromParent(pThis->unk_20, ".WeaponPower.ScrollableInfo.WeaponDetailsStack.WeaponPowerDetails");
        pChild = (Unk19aec0 *)operator new(sizeof(Unk19aec0));
        sub_710019aec0(pChild, 0, idWindow, idVisual, idScrollBox, idName, idDetails, 44.0f, 91.0f, false);
        pOld = pThis->unk_00[0];
        pThis->unk_00[0] = pChild;
        delete pOld;
    }
    {
        UIID idWindow = UIIDCreateFromParent(pThis->unk_20, ".ArmorPower");
        UIID idVisual = UIIDCreateFromParent(pThis->unk_20, ".ArmorPower.ItemVisual");
        UIID idScrollBox = UIIDCreateFromParent(pThis->unk_20, ".ArmorPower.ScrollableInfo");
        UIID idName = UIIDCreateFromParent(pThis->unk_20, ".ArmorPower.ScrollableInfo.ArmorDetailsStack.ArmorPowerName");
        UIID idDetails = UIIDCreateFromParent(pThis->unk_20, ".ArmorPower.ScrollableInfo.ArmorDetailsStack.ArmorPowerDetails");
        pChild = (Unk19aec0 *)operator new(sizeof(Unk19aec0));
        sub_710019aec0(pChild, 1, idWindow, idVisual, idScrollBox, idName, idDetails, 287.0f, 91.0f, false);
        pOld = pThis->unk_00[1];
        pThis->unk_00[1] = pChild;
        delete pOld;
    }
    {
        UIID idWindow = UIIDCreateFromParent(pThis->unk_20, ".JewelryPower");
        UIID idVisual = UIIDCreateFromParent(pThis->unk_20, ".JewelryPower.ItemVisual");
        UIID idScrollBox = UIIDCreateFromParent(pThis->unk_20, ".JewelryPower.ScrollableInfo");
        UIID idName = UIIDCreateFromParent(pThis->unk_20, ".JewelryPower.ScrollableInfo.JewelryDetailsStack.JewelryPowerName");
        UIID idDetails = UIIDCreateFromParent(pThis->unk_20, ".JewelryPower.ScrollableInfo.JewelryDetailsStack.JewelryPowerDetails");
        pChild = (Unk19aec0 *)operator new(sizeof(Unk19aec0));
        sub_710019aec0(pChild, 2, idWindow, idVisual, idScrollBox, idName, idDetails, 44.0f, 308.0f, false);
        pOld = pThis->unk_00[2];
        pThis->unk_00[2] = pChild;
        delete pOld;
    }
    {
        UIID idWindow = UIIDCreateFromParent(pThis->unk_20, ".UniversalPower");
        UIID idVisual = UIIDCreateFromParent(pThis->unk_20, ".UniversalPower.ItemVisual");
        UIID idScrollBox = UIIDCreateFromParent(pThis->unk_20, ".UniversalPower.ScrollableInfo");
        UIID idName = UIIDCreateFromParent(pThis->unk_20, ".UniversalPower.ScrollableInfo.UniversalDetailsStack.UniversalPowerName");
        UIID idDetails = UIIDCreateFromParent(pThis->unk_20, ".UniversalPower.ScrollableInfo.UniversalDetailsStack.UniversalPowerDetails");
        pChild = (Unk19aec0 *)operator new(sizeof(Unk19aec0));
        sub_710019aec0(pChild, 3, idWindow, idVisual, idScrollBox, idName, idDetails, 287.0f, 308.0f, true);
        pOld = pThis->unk_00[3];
        pThis->unk_00[3] = pChild;
        delete pOld;
    }
}

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

// Probably: Kanai's powers panel refresh from the player's equipped Kanai's cube powers.
extern "C" void sub_710019bab0(Unk19b630 * pThis, const Player * ptPlayer)
{
    BOOL bFourthSlotUnlocked = sub_710024d7e0();
    UI_GetWindowFromID(pThis->unk_28).SetActive(bFourthSlotUnlocked);
    for (int32 i = 0; i < 4; i++)
    {
        GBID gbidPower = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[i];
        sub_710019b0b0(pThis->unk_00[i], gbidPower, bFourthSlotUnlocked, -1, TRUE);
    }
}
