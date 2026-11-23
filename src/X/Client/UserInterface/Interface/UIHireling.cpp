// Original file: X/Client/UserInterface/Interface/UIHireling.cpp
#include "UIHirelingCommon.h"
#include "UIGame.h"
#include "UITutorials.h"
#include "../../CPlayer.h"
#include "../../SNOPreload.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HirelingSwig.h"
// DWARF UIHireling.cpp:152,284,340,420,431,614,679,783,1214.
namespace UIHireling {
void OnInit(const UIID &); void ToggleOnClick(const UIID &); void OnFollowerDismissed(ACDID);
void Close(); void Display(); void PopulateHirelingDetails(const ActorCommonData &, const UIID &, int32);
void DetailsOnClick(const UIID &); ACDID GetCurrentHireling();
}
// DWARF UISideScreen.cpp declaration; UIHireling.cpp:625.
void UISideScreenSetVisible(const UIID &, BOOL); void sShowHirelingDetails(BOOL);
// DWARF UITutorials.cpp global manager and UIIDs.h generated control declarations.
extern UITutorialManager gtTutorialMgr;
extern UIID UIDLG_HIRELING_PAGE, UIDLG_HIRELING_DETAILS_DLG_CONTAINER;
extern UIID UIBUTTON_HIRELING_SKILLS_0_0, UIBUTTON_HIRELING_SKILLS_1_0;
extern UIID UIBUTTON_HIRELING_SKILLS_0_1, UIBUTTON_HIRELING_SKILLS_1_1;
extern UIID UIBUTTON_HIRELING_SKILLS_0_2, UIBUTTON_HIRELING_SKILLS_1_2;
extern UIID UIBUTTON_HIRELING_SKILLS_0_3, UIBUTTON_HIRELING_SKILLS_1_3;
extern UIID UIBUTTON_HIRELING_SKILLS_0_4, UIBUTTON_HIRELING_SKILLS_1_4;
#include "../../Gfx/GfxVB.h"  // Inference: original global initializer stores the GfxVB.h:351-354 normal constants before the RGBA colors.
#include "../Controls/UIScrollbarControl.h"
#include "../Controls/UIStackPanelControl.h"
// DWARF generated UIIDs.h declaration.
extern UIID UIDLG_HIRELING_DETAILS_DLG;
// DWARF UI.cpp:2628 returns UIStackPanel &.
UIStackPanel & UI_GetStackPanelFromID(const UIID &);
#include "../../CPlayerInput.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Hireling.h"
#include "../../../Sigma/RGBAColor.h"
// DWARF CPlayerInput.cpp; UIDraw.cpp; ActorInventory.cpp:552,11712; UIInventory.cpp.
CursorMgr & PlayerGetCursorMgr();
extern const float UIDC_DIALOG_Z_START, UIDC_DIALOG_Z_INCREMENT;
BOOL ACDInventoryIsSlotValidForHireling(const ActorCommonData &, int32, InventorySlotType);
void ACDInventoryWhereCanThisGoHireling(GBID, int32 &, InventorySlotType *);
namespace UIInventory {
void SlotDrawItems(ActorCommonData &, PlayerClass, BOOL, InventorySlotType, float &, int32);
void TrackItemDrop(const ActorCommonData &, InventorySlotType &, IVector2D &);
void HighlightSlot(InventorySlotType, const IVector2D &, const ActorCommonData &, const ActorCommonData &, float &);
void DrawSlotHighlights(InventorySlotType, const RGBAColor &, float &, int32);
ACDID GetHighlightedItem();
}
namespace UIHireling { void Draw(uint8, float &); }
#include "../../RActors/RActors.h"
// DWARF ActorCommonData.cpp:8221, VisualInventory.cpp:1181, UIInventoryCommon.cpp:498.
BOOL ACDIsHirelingProxy(const ActorCommonData &); void VisualInventoryCopy(ACDID, ACDID);
namespace UIInventory { void HandleQuickMoveToHireling(ActorCommonData &, ActorCommonData &); }
// DWARF UIHireling.cpp:1170,1222.
namespace UIHireling { void Bkgrnd_OnClick(const UIID &); void UpdateVisualEquipmentForHirelingNPC(const ActorCommonData &); }
// Inference: both RGBA initializers follow the original global initializer byte stores (red/white, alpha128).
static const RGBAColor sgrgbaUnusableHighlight(255, 0, 0, 128);
static const RGBAColor sgrgbaWhite(255, 255, 255, 128);
#include "../../../Sigma/ErrorManager/ErrorManager.h"
// DWARF UIHireling.cpp:1100.
namespace UIHireling { void ChooseOnClick(const UIID &); }
#include "../../CGame.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIBlinkerControl.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/Math/MathGeneral.h"

struct HirelingSkillButtonID {
    const UIID * pidButton;
    UIID idBlinker;
};


struct UIHirelingGlobals {

    ACDID idACDCurrentHireling;
    ACDID idACDNpcToUpdate;
    HirelingClass eCurrentClass;
    SNO snoSkillToLearn;
};

static UIHirelingGlobals sgtUIHirelingGlobals;



// Inference: paired button pointers and GetUIIDNone initializers follow original global initializer calls.
static HirelingSkillButtonID sgtHirelingSkillButtonIDs[5][2] = {
    {{ &UIBUTTON_HIRELING_SKILLS_0_0, GetUIIDNone() }, { &UIBUTTON_HIRELING_SKILLS_1_0, GetUIIDNone() }},
    {{ &UIBUTTON_HIRELING_SKILLS_0_1, GetUIIDNone() }, { &UIBUTTON_HIRELING_SKILLS_1_1, GetUIIDNone() }},
    {{ &UIBUTTON_HIRELING_SKILLS_0_2, GetUIIDNone() }, { &UIBUTTON_HIRELING_SKILLS_1_2, GetUIIDNone() }},
    {{ &UIBUTTON_HIRELING_SKILLS_0_3, GetUIIDNone() }, { &UIBUTTON_HIRELING_SKILLS_1_3, GetUIIDNone() }},
    {{ &UIBUTTON_HIRELING_SKILLS_0_4, GetUIIDNone() }, { &UIBUTTON_HIRELING_SKILLS_1_4, GetUIIDNone() }}
};


// DWARF UIIDs.h generated declarations and UIHireling.cpp:897 forward declaration.
extern UIID UITEXT_HIRE_LEVEL, UITEXT_HIRE_NAME, UITEXT_HIRE_LIFE, UITEXT_HIRE_DAMAGE, UITEXT_HIRE_SURVIVABILITY;
extern UIID UITEXT_HIRE_STRENGTH, UITEXT_HIRE_DEXTERITY, UITEXT_HIRE_INTELLIGENCE, UITEXT_HIRE_VITALITY;
extern UIID UIWND_HIRE_BACKGROUND, UIWND_HIRE_SPECIAL_PADLOCK, UITXT_HIRE_SPECIAL_PADLOCK, UIBTN_HIRE_LEFTARM;
static void sUpdateSkills();
// DWARF Monsters.cpp, UIHirelingCommon.cpp, ActorInventory.cpp, Player.cpp.
CRefString MonsterGetName(const ActorCommonData * ptACD, const BOOL bSetTextColor);  // Monsters.cpp:641
namespace UIHireling { CRefString GetNameForHirelingClass(HirelingClass, BOOL); }
BOOL ACDInventoryHirelingSlotUnlocked(const ActorCommonData &);
ACDID PlayerGetHirelingProxy(ACDID, HirelingClass);
#include "../../../Common/Definitions/GlobalDefinition.h"
// DWARF UIUtils.cpp:845.
CRefString FormatTruncatedNumber(float, BOOL, BOOL);
#include "../../../Common/Powers/PowerAPISwig.h"

































void UIHireling::Draw(
    const uint8,
    float & flZSlice)
{

    if (!UI_TestID(UIDLG_HIRELING_PAGE)) return;

    Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
        return;



    ACDID idACDPlayer = ptPlayer->idPlayerACD;
    if (idACDPlayer == ACDID_INVALID)
        return;



    ACDID idACDHireling = sgtUIHirelingGlobals.idACDCurrentHireling;
    if (idACDHireling == ACDID_INVALID)
        return;



    ActorCommonData & tACDHireling = ACDGet(idACDHireling);
    float flLayerZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;
    float flZToUse = flLayerZ;

    HirelingClass eHC = HirelingGetClass(tACDHireling);
    PlayerClass eClass = PlayerGetPlayerClass(idACDPlayer);
    BOOL fIsMale = PlayerIsMale(*ptPlayer);

    // Inference: inventory slots20..25 follow the six unrolled original validity checks.
    for (int32 i = SLOT_HIRELING_START; i < SLOT_HIRELING_END; ++i)
    {
        InventorySlotType eSlot = (InventorySlotType)i;
        if (ACDInventoryIsSlotValidForHireling(tACDHireling, eHC, eSlot))
        {
            UIInventory::SlotDrawItems(tACDHireling, eClass, fIsMale, eSlot, flZToUse, 0);
        }
    }

    ActorCommonData * ptACDHand = PlayerGetCursorMgr().GetItemToMove();
    if (ptACDHand)
    {



        InventorySlotType eSlot;
        IVector2D tSlot;
        UIInventory::TrackItemDrop(*ptACDHand, eSlot, tSlot);
        if (ACDInventoryIsSlotValidForHireling(tACDHireling, eHC, eSlot))
        {
            UIInventory::HighlightSlot(eSlot, tSlot, tACDHireling, *ptACDHand, flZToUse);
        }


        int32 nNumSlots = 0;
        InventorySlotType eValidSlots[4];
        ACDInventoryWhereCanThisGoHireling(ptACDHand->hGB.gbid, nNumSlots, eValidSlots);

        for (int32 i = 0; i < nNumSlots; ++i)
        {
            if (eValidSlots[i] == eSlot)
                continue;



            if (!ACDInventoryIsSlotValidForHireling(tACDHireling, eHC, eValidSlots[i]))
                continue;



            InventoryLocation tInvLoc;
            tInvLoc.idACDContainer = idACDHireling;
            tInvLoc.eSlot = eValidSlots[i];
            tInvLoc.pt = cgptNull;


            RGBAColor rgbaColor;
            if (ACDInventoryItemAllowedInSlot(*ptACDHand, tInvLoc, FALSE, FALSE) == GAMEERROR_NONE)
            {
                rgbaColor = sgrgbaWhite;
            }
            else
            {
                rgbaColor = sgrgbaUnusableHighlight;
            }
            UIInventory::DrawSlotHighlights(eValidSlots[i], rgbaColor, flZToUse, 0);
        }
    }
    else
    {

        ACDID idACDHighlightedItem = UIInventory::GetHighlightedItem();
        if (idACDHighlightedItem != ACDID_INVALID)
        {
            ActorCommonData & tACDHighlightedItem = ACDGet(idACDHighlightedItem);



            int32 nNumSlots = 0;
            InventorySlotType eValidSlots[4];
            ACDInventoryWhereCanThisGoHireling(tACDHighlightedItem.hGB.gbid, nNumSlots, eValidSlots);

            for (int32 i = 0; i < nNumSlots; ++i)
            {
                InventoryLocation tInvLoc;
                tInvLoc.idACDContainer = idACDHireling;
                tInvLoc.eSlot = eValidSlots[i];
                tInvLoc.pt = cgptNull;


                RGBAColor rgbaColor;
                if (ACDInventoryItemAllowedInSlot(tACDHighlightedItem, tInvLoc, FALSE, FALSE) == GAMEERROR_NONE)
                {
                    rgbaColor = sgrgbaWhite;
                }
                else
                {
                    rgbaColor = sgrgbaUnusableHighlight;
                }
                UIInventory::DrawSlotHighlights(eValidSlots[i], rgbaColor, flZToUse, 0);
            }
        }
    }
}



void UIHireling::OnInit(
    const UIID &)
{

    sgtUIHirelingGlobals.idACDCurrentHireling = ACDID_INVALID;
    sgtUIHirelingGlobals.idACDNpcToUpdate = ACDID_INVALID;

}



static void sCreateControls()
{
    // Inference: 5 rows and 2 choices follow DWARF array extents and ten unrolled original calls.
    for (int32 nRow = 0; nRow < 5; ++nRow)
    {
        for (int32 nChoice = 0; nChoice < 2; ++nChoice)
        {
            const UIID & idButton = *sgtHirelingSkillButtonIDs[nRow][nChoice].pidButton;

            sgtHirelingSkillButtonIDs[nRow][nChoice].idBlinker = UIIDCreateFromParent(idButton, ".blinker");
        }
    }
}










static void sOpenForHireling(
    ACDID idACDHireling,
    ACDID idACDNpc,
    HirelingClass eClass)
{
    sgtUIHirelingGlobals.idACDCurrentHireling = idACDHireling;
    sgtUIHirelingGlobals.idACDNpcToUpdate = idACDNpc;
    sgtUIHirelingGlobals.eCurrentClass = eClass;


    UISideScreenSetVisible(UIDLG_HIRELING_PAGE, TRUE);



    sCreateControls();


    UIHireling::Display();
}



void UIHireling::ToggleOnClick(
    const UIID &)
{







    if (!UI_TestID(UIDLG_HIRELING_PAGE))
        return;

    if (UIGame::MenuIsActive())
        return;




    ACDID idCurrentHireling = PlayerGetHireling(LocalPlayerACDGet());
    ActorCommonData * ptACDHireling = ACDTryToGet(idCurrentHireling);

    // Inference: null-hireling early return is the line-0 exit after the original inlined lookup.
    if (!ptACDHireling) return;



    if (!UI_GetWindowFromID(UIDLG_HIRELING_PAGE).IsActive())
    {
        sOpenForHireling(idCurrentHireling, ACDID_INVALID, ACDGetHirelingClass(*ptACDHireling));
    }
    else
    {
        Close();
    }
}

namespace UIHireling { void OpenFromMenu(ACDID, HirelingClass); }
// LINKER-REMOVED: DWARF UIHireling.cpp:379 concrete DIE (low_pc 0, 0x11c bytes); body from its line table and location lists.
void UIHireling::OpenFromMenu(
    ACDID idACDNpc,
    HirelingClass eClass)
{
    // Inference: 384 col5 (5 insns) and 387 col9 (2 insns) mirror ToggleOnClick's UI_TestID / MenuIsActive early returns.
    if (!UI_TestID(UIDLG_HIRELING_PAGE))
        return;

    if (UIGame::MenuIsActive())
        return;



    ACDID idACDLocalPlayer = LocalPlayerACDGet();

    // Inference: one-argument call at col34 returning the current hireling, as in ToggleOnClick.
    ACDID idACDCurrentHireling = PlayerGetHireling(idACDLocalPlayer);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDCurrentHireling);
    if (ptACDHireling)
    {
        if (ACDGetHirelingClass(*ptACDHireling) == eClass)
        {
            // Inference: 403 col13 is a tail call taking (w22 idACDCurrentHireling, w20 idACDNpc, w19 eClass).

            sOpenForHireling(idACDCurrentHireling, idACDNpc, eClass);
            return;
        }
    }




    ACDID idACDProxy = PlayerGetHirelingProxy(idACDLocalPlayer, eClass);
    sOpenForHireling(idACDProxy, idACDNpc, eClass);

    // NOT WRITTEN: line 415 col5 is a one-argument tail call (one setup insn) whose target is unrecoverable from the dropped function.

}



void UIHireling::OnFollowerDismissed(
    ACDID idACDHireling)
{
    if (sgtUIHirelingGlobals.idACDCurrentHireling == idACDHireling)
    {
        Close();
    }
}



void UIHireling::Close()

{
    UISideScreenSetVisible(UIDLG_HIRELING_PAGE, FALSE);
    sShowHirelingDetails(FALSE);


    sgtUIHirelingGlobals.idACDCurrentHireling = ACDID_INVALID;
    sgtUIHirelingGlobals.idACDNpcToUpdate = ACDID_INVALID;
    sgtUIHirelingGlobals.eCurrentClass = HC_NONE;








    gtTutorialMgr.CloseTutorialIfOpen(GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIP_HIRELING));

    SNOPreload::UpdateHireling();
    SNOPreload::UpdateSkills();
    SNOPreload::UpdateEquipped();
}




static void sSetDetailValues(
    const ActorCommonData & tACD,
    const UIID & idHirelingDetails,
    int32 nLevel)
{
    UIID idHirelingDetailsRegionStack;
    idHirelingDetailsRegionStack.SetFromParent(idHirelingDetails, "region.stackpanel");

    {
        float flStat = PowerComputeDamageScalarFromPrimaryAttribute(tACD.idFastAttrib);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        UIID idTextOffenseDamageBonus;
        idTextOffenseDamageBonus.SetFromParent(idHirelingDetailsRegionStack, "category_offense.stackpanel.damage_increase.val");
        UI_GetTextFromID(idTextOffenseDamageBonus).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        UIID idTextOffenseAttackPerSecond;
        idTextOffenseAttackPerSecond.SetFromParent(idHirelingDetailsRegionStack, "category_offense.stackpanel.attacks_per_second.val");
        UI_GetTextFromID(idTextOffenseAttackPerSecond).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = PowerComputeCritChance(&tACD, FALSE);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        UIID idTextOffenseCritChance;
        idTextOffenseCritChance.SetFromParent(idHirelingDetailsRegionStack, "category_offense.stackpanel.crit_chance_bonus.val");
        UI_GetTextFromID(idTextOffenseCritChance).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = PowerComputeCritDamageScalar(&tACD) - 1.0f;
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        UIID idTextOffenseCritDamage;
        idTextOffenseCritDamage.SetFromParent(idHirelingDetailsRegionStack, "category_offense.stackpanel.crit_damage_bonus.val");
        UI_GetTextFromID(idTextOffenseCritDamage).SetText(usStat.str(), FALSE);
    }

    {
        float flStatMin = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MIN);
        float flStatMax = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MAX);
        CRefString sStatMin = FormatTruncatedNumber(flStatMin, FALSE, FALSE);
        CRefString sStatMax = FormatTruncatedNumber(flStatMax, FALSE, TRUE);
        CRefString usStat = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RangeFormat"),
            sStatMin.str(), sStatMax.str(), NULL);

        UIID idTextDefenseBlockAmount;
        idTextDefenseBlockAmount.SetFromParent(idHirelingDetailsRegionStack, "category_defense.stackpanel.block_amount.val");
        UI_GetTextFromID(idTextDefenseBlockAmount).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_CHANCE_CAPPED_TOTAL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

        UIID idTextDefenseBlockChance;
        idTextDefenseBlockChance.SetFromParent(idHirelingDetailsRegionStack, "category_defense.stackpanel.block_chance.val");
        UI_GetTextFromID(idTextDefenseBlockChance).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = Math::Max(PowerComputeDodgeChance(tACD, nLevel, PDM_MELEE, FALSE), 0.0f);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

        UIID idTextDefenseDodgeChance;
        idTextDefenseDodgeChance.SetFromParent(idHirelingDetailsRegionStack, "category_defense.stackpanel.dodge_chance.val");
        UI_GetTextFromID(idTextDefenseDodgeChance).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = PowerComputeArmorDamageMitigation(tACD, nLevel);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        UIID idTextDefenseDamageReduction;
        idTextDefenseDamageReduction.SetFromParent(idHirelingDetailsRegionStack, "category_defense.stackpanel.damage_reduction.val");
        UI_GetTextFromID(idTextDefenseDamageReduction).SetText(usStat.str(), FALSE);
    }










    {
        int32 nValMax = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
        CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);

        UIID idTextLifeLife;
        idTextLifeLife.SetFromParent(idHirelingDetailsRegionStack, "category_life.stackpanel.life.val");
        UI_GetTextFromID(idTextLifeLife).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_SUBTOTAL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        UIID idTextLifeLifePerSecond;
        idTextLifeLifePerSecond.SetFromParent(idHirelingDetailsRegionStack, "category_life.stackpanel.life_per_second.val");
        UI_GetTextFromID(idTextLifeLifePerSecond).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        UIID idTextLifeLifeSteal;
        idTextLifeLifeSteal.SetFromParent(idHirelingDetailsRegionStack, "category_life.stackpanel.life_steal.val");
        UI_GetTextFromID(idTextLifeLifeSteal).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_KILL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        UIID idTextLifeLifePerKill;
        idTextLifeLifePerKill.SetFromParent(idHirelingDetailsRegionStack, "category_life.stackpanel.life_per_kill.val");
        UI_GetTextFromID(idTextLifeLifePerKill).SetText(usStat.str(), FALSE);
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        UIID idTextLifeLifePerHit;
        idTextLifeLifePerHit.SetFromParent(idHirelingDetailsRegionStack, "category_life.stackpanel.life_per_hit.val");
        UI_GetTextFromID(idTextLifeLifePerHit).SetText(usStat.str(), FALSE);
    }


    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRatio = pdefGlobal->flHirelingDiminishedBonusRatio;
        float flStat = flRatio * tACD.AttributesGetFloat(ATTRIBUTE_GOLD_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);

        UIID idTextAdventurerGoldFind;
        idTextAdventurerGoldFind.SetFromParent(idHirelingDetailsRegionStack, "category_adventurer.stackpanel.gold_find.val");
        UI_GetTextFromID(idTextAdventurerGoldFind).SetText(usStat.str(), FALSE);
    }

    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRatio = pdefGlobal->flHirelingDiminishedBonusRatio;
        float flStat = flRatio * tACD.AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);

        UIID idTextAdventurerMagicFind;
        idTextAdventurerMagicFind.SetFromParent(idHirelingDetailsRegionStack, "category_adventurer.stackpanel.magic_find.val");
        UI_GetTextFromID(idTextAdventurerMagicFind).SetText(usStat.str(), FALSE);
    }

}



void UIHireling::PopulateHirelingDetails(
    const ActorCommonData & tACD,
    const UIID & idHirelingDetails,
    int32 nLevel)
{
    sSetDetailValues(tACD, idHirelingDetails, nLevel);
}




void sShowHirelingDetails(
    const BOOL fShow)
{
    UIWindow & tDetailsDlgContainer = UI_GetWindowFromID(UIDLG_HIRELING_DETAILS_DLG_CONTAINER);
    BOOL fChanged = tDetailsDlgContainer.IsActive() != fShow;
    tDetailsDlgContainer.SetActive(fShow);

    if (fChanged)
    {
        SoundManager::PlaySound(GlobalSNOGet(fShow ? GLOBALSNO_SOUND_TABMENU_OPEN : GLOBALSNO_SOUND_TABMENU_CLOSE), cgtPSODefaults);
    }

    if (fShow)
    {
        ActorCommonData * ptACD = ACDTryToGet(sgtUIHirelingGlobals.idACDCurrentHireling);
        // Inference: null-ACD early return follows the original inlined lookup exit.
        if (!ptACD) return;



        int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
        UIHireling::PopulateHirelingDetails(*ptACD, UIDLG_HIRELING_DETAILS_DLG, nLevel);

        Vector2D vecRegionTL;
        Vector2D vecRegionDim;
        UIID idHirelingDetailsRegion;
        idHirelingDetailsRegion.SetFromParent(UIDLG_HIRELING_DETAILS_DLG, "region");
        UI_GetWindowFromID(idHirelingDetailsRegion).GetRelativePosition(vecRegionTL, vecRegionDim);

        UIID idHirelingDetailsScrollbar;
        idHirelingDetailsScrollbar.SetFromParent(UIDLG_HIRELING_DETAILS_DLG, "Scrollbar");
        UIScrollbar & tScrollbar = UI_GetScrollbarFromID(idHirelingDetailsScrollbar);
        tScrollbar.SetOwner(idHirelingDetailsRegion);
        Vector2D vecStackTL;
        Vector2D vecStackDim;
        UIID idHirelingDetailsRegionStack;
        idHirelingDetailsRegionStack.SetFromParent(idHirelingDetailsRegion, "stackpanel");
        UI_GetStackPanelFromID(idHirelingDetailsRegionStack).GetRelativePosition(vecStackTL, vecStackDim);
        // Inference: original comparison uses the 1e-6 float literal.
        float flExtents = vecStackDim.y - vecRegionDim.y;
        if (flExtents < 0.000001f) {
            // Inference: zero extents and page increment10 follow original immediates.
            tScrollbar.SetExtentsAndPosition(0, 0, 0, TRUE);
        }
        else
        {
            tScrollbar.SetExtentsAndPosition(0, (int32)flExtents, 0, TRUE);
            tScrollbar.SetIncrements(10, (int32)vecRegionDim.y);
        }
    }
}



void UIHireling::DetailsOnClick(
    const UIID &)
{
    BOOL fShow = !UI_GetWindowFromID(UIDLG_HIRELING_DETAILS_DLG_CONTAINER).IsActive();
    sShowHirelingDetails(fShow);
}



static void sSetStatText(
    const UIID & uId,
    CRefString & uzString)
{
    if (!UI_TestID(uId))
        return;



    UIText & tText = UI_GetTextFromID(uId);
    tText.SetText(uzString, FALSE);
}



static void sDisplayHirelingMural(
    HirelingClass eClass)
{

    CRefString sLabel;
    switch (eClass)
    {
    case HC_TEMPLAR:
        sLabel = "UI:FollowerBaseMural_Templar";
        break;
    case HC_SCOUNDREL:
        sLabel = "UI:FollowerBaseMural_Scoundrel";
        break;
    case HC_ENCHANTRESS:
        sLabel = "UI:FollowerBaseMural_Enchantress";
        break;
    }

    UI_GetWindowFromID(UIWND_HIRE_BACKGROUND).SetTextureFrame(sLabel);
}




static void sDisplayHirelingStats(
    ActorCommonData & tACDHireling)
{

    int32 nLevel = tACDHireling.AttributesGetInt(ATTRIBUTE_LEVEL);

    HirelingClass eClass = HirelingGetClass(tACDHireling);
    CRefString usClass = UIHireling::GetNameForHirelingClass(eClass, FALSE);


    CRefString usLevel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PETS), "HirelingLevel");
    usLevel = StringListEntryReplace(usLevel, "{LEVEL}", CRefString().Format("%d", nLevel), 999);
    usLevel = StringListEntryReplace(usLevel, "{CLASS}", usClass.str(), 999);
    sSetStatText(UITEXT_HIRE_LEVEL, usLevel);

    CRefString usName = MonsterGetName(&tACDHireling, FALSE);
    sSetStatText(UITEXT_HIRE_NAME, usName);

    int32 nValCur = Math::FloatRoundToInt(Math::Max(tACDHireling.AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR), 0.0f));
    int32 nValMax = Math::FloatRoundToInt(tACDHireling.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
    CRefString usLife = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PETS), "HirelingLife"),
        UnicodeText::FormatNumber(nValCur, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
        UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    sSetStatText(UITEXT_HIRE_LIFE, usLife);

    float flDamage, flAttackSpeed;
    PowerActorGetWeaponDamageAndAttackSpeed(tACDHireling, flDamage, flAttackSpeed, TRUE, POWERID_INVALID, TRUE);
    float flBaseDamageScalar = UIHireling::GetBaseDamageScalarForClass(tACDHireling);
    float flDPS = flDamage * flAttackSpeed * flBaseDamageScalar;

    CRefString usDPS = UnicodeText::FormatNumber(flDPS, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_DAMAGE, usDPS);

    float flEHP = tACDHireling.GetEHPValue();
    CRefString usSurvivability = UnicodeText::FormatNumber(flEHP, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_SURVIVABILITY, usSurvivability);

    int32 nStrength = Math::FloatRoundToInt(tACDHireling.AttributesGetFloat(ATTRIBUTE_STRENGTH_TOTAL));
    CRefString usStrength = UnicodeText::FormatNumber(nStrength, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_STRENGTH, usStrength);

    int32 nDexterity = Math::FloatRoundToInt(tACDHireling.AttributesGetFloat(ATTRIBUTE_DEXTERITY_TOTAL));
    CRefString usDexterity = UnicodeText::FormatNumber(nDexterity, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_DEXTERITY, usDexterity);

    int32 nIntelligence = Math::FloatRoundToInt(tACDHireling.AttributesGetFloat(ATTRIBUTE_INTELLIGENCE_TOTAL));
    CRefString usIntelligence = UnicodeText::FormatNumber(nIntelligence, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_INTELLIGENCE, usIntelligence);

    int32 nVitality = Math::FloatRoundToInt(tACDHireling.AttributesGetFloat(ATTRIBUTE_VITALITY_TOTAL));
    CRefString usVitality = UnicodeText::FormatNumber(nVitality, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    sSetStatText(UITEXT_HIRE_VITALITY, usVitality);
}



void UIHireling::Display()

{
    // Inference: zero initialization and four-tick throttle follow the original static BSS and comparison.
    static uint32 sdwLastUpdate;
    uint32 dwCurrTime = CGameGetRealTicks();
    if (dwCurrTime - sdwLastUpdate < 4)
        return;


    if (!UI_TestID(UIDLG_HIRELING_PAGE)) return;
    if (!UI_GetWindowFromID(UIDLG_HIRELING_PAGE).IsActive())
    {
        sdwLastUpdate = 0;
        return;
    }
    sdwLastUpdate = dwCurrTime;


    ActorCommonData * ptACDHireling = ACDTryToGet(sgtUIHirelingGlobals.idACDCurrentHireling);
    if (!ptACDHireling)
    {


        ACDID idACDLocalPlayer = LocalPlayerACDGet();
        sgtUIHirelingGlobals.idACDCurrentHireling = PlayerGetHirelingProxy(idACDLocalPlayer, sgtUIHirelingGlobals.eCurrentClass);
        ptACDHireling = ACDTryToGet(sgtUIHirelingGlobals.idACDCurrentHireling);
    }


    if (!ptACDHireling)
    {
        sdwLastUpdate = 0;
    }


    sDisplayHirelingMural(sgtUIHirelingGlobals.eCurrentClass);
    if (ptACDHireling)
    {
        sDisplayHirelingStats(*ptACDHireling);



        if (UI_GetWindowFromID(UIDLG_HIRELING_DETAILS_DLG).IsActive())
        {
            int32 nLevel = ptACDHireling->AttributesGetInt(ATTRIBUTE_LEVEL);
            PopulateHirelingDetails(*ptACDHireling, UIDLG_HIRELING_DETAILS_DLG, nLevel);
        }
    }

    UIWindow & tWndPadlock = UI_GetWindowFromID(UIWND_HIRE_SPECIAL_PADLOCK);
    if (ptACDHireling && ACDInventoryHirelingSlotUnlocked(*ptACDHireling))
    {
        tWndPadlock.SetActive(FALSE);
    }
    else
    {
        tWndPadlock.SetActive(TRUE);
        UI_GetTextFromID(UITXT_HIRE_SPECIAL_PADLOCK).SetText(CRefString().Format("%d", 18), FALSE);
    }

    sUpdateSkills();

    if (ptACDHireling)
    {
        if (HirelingHasUnspentSkillPoints(*ptACDHireling))
        {
            gtTutorialMgr.ShowTutorial(UIBUTTON_HIRELING_SKILLS_1_0, GlobalSNOGet(GLOBALSNO_TUTORIAL_HIRELING_TRAINING), NULL, TRUE);
            gtTutorialMgr.QueueTutorial(UIBTN_HIRE_LEFTARM, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIP_HIRELING), 0, -1);
        }
        else
        {
            gtTutorialMgr.ShowTutorial(UIBTN_HIRE_LEFTARM, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIP_HIRELING), NULL, TRUE);
        }
    }
}



static SNO sGetPowerFromButton(
    ActorCommonData * ptACDHireling,
    const UIID & id)
{
    // Inference: null-hireling return path; Tooltip (DWARF 1062->1063) has no line for a caller guard, and the original inlines this helper, so it was not noreturn before inlining.
    if (!ptACDHireling) return INVALID_SNO;





















    // Inference: the surviving Switch path is an unconditional fatal error; no eliminated gameplay is reconstructed.
    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}





static void sUpdateSkills()
{


    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;



    ACDID idACDHireling = sgtUIHirelingGlobals.idACDCurrentHireling;
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (!ptACDHireling)
        return;



    SNO snoSkillKit = ptACDHireling->GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    int32 nUnlockLevelBase = HirelingGetUnlockLevelBase(pdefSkillKit);
    // Inference: five-by-two loop bounds follow the DWARF table; the first skill terminates in the original fatal path.
    for (int32 nRow = 0; nRow < 5; ++nRow)
    {

        HirelingTrainReason eSkillReason;
        SNO snoLearnedPower;

        for (int32 nChoice = 0; nChoice < 2; ++nChoice)
        {
            const UIID & idButton = *sgtHirelingSkillButtonIDs[nRow][nChoice].pidButton;
            if (!UI_TestID(idButton)) return;
            UIButton & tButton = UI_GetButtonFromID(idButton);

            const UIID & idBlinker = sgtHirelingSkillButtonIDs[nRow][nChoice].idBlinker;
            if (!UI_TestID(idBlinker)) return;
            UIBlinker & tBlinker = UI_GetBlinkerFromID(idBlinker);

            BOOL bActive;
            BOOL bBlinkerActive;

            SNO snoPower = sGetPowerFromButton(ptACDHireling, idButton);
        }
    }
}











































































































void UIToolTips_HirelingSkillButtons(
    const UIID & id)
{

    if (!UI_TestID(id)) return;


    ACDID idACD = LocalPlayerACDGet();
    if (idACD == ACDID_INVALID)
        return;



    ACDID idACDHireling = sgtUIHirelingGlobals.idACDCurrentHireling;
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    SNO snoPower = sGetPowerFromButton(ptACDHireling, id);










}

























void UIHireling::ChooseOnClick(
    const UIID & id)
{

    if (!UI_TestID(id)) return;


    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;



    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;



    ACDID idACDHireling = sgtUIHirelingGlobals.idACDCurrentHireling;
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (!ptACDHireling)
        return;

    // Inference: no eliminated post-fatal gameplay is reconstructed (DWARF locals eReason:1136 left unlocated).
    SNO snoPower = sGetPowerFromButton(ptACDHireling, id);










    HirelingTrainReason eReason;





























}



void UIHireling::Bkgrnd_OnClick(
    const UIID &)
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (tCursorMgr.GetCursorState() != CURSORSTATE_MOVE_ITEM)
        return;



    ACDID idACDHireling = sgtUIHirelingGlobals.idACDCurrentHireling;
    if (idACDHireling == ACDID_INVALID)
        return;



    ActorCommonData * ptACDHandItem = PlayerGetCursorMgr().GetItemToMove();

    ActorCommonData & tACDHireling = ACDGet(idACDHireling);
    UIInventory::HandleQuickMoveToHireling(tACDHireling, *ptACDHandItem);
}
























ACDID UIHireling::GetCurrentHireling()
{
    return sgtUIHirelingGlobals.idACDCurrentHireling;
}




void UIHireling::UpdateVisualEquipmentForHirelingNPC(
    const ActorCommonData & tACD)
{
    Player * ptPlayer = tACD.GetPetOwnerPlayer();
    if (ptPlayer && PlayerIsLocal(ptPlayer))
    {
        if (ACDIsHirelingProxy(tACD))
        {
            HirelingClass eClass = ACDGetHirelingClass(tACD);

            if (sgtUIHirelingGlobals.idACDNpcToUpdate != ACDID_INVALID)
            {
                if (eClass != HC_NONE && eClass == sgtUIHirelingGlobals.eCurrentClass)
                {
                    VisualInventoryCopy(tACD.id, sgtUIHirelingGlobals.idACDNpcToUpdate);
                }
            }
            else
            {
                ACDIterator tIt;
                while (ActorCommonDataIterate(tIt))
                {
                    ActorCommonData * ptACD = tIt.ptElement;


                    HirelingClass eClassOther = ACDGetHirelingClass(*ptACD);
                    if (eClass == eClassOther && !ACDIsHireling(*ptACD)) {
                        // Inference: the owner DataID is the RActorID passed to the DWARF-inlined RActorTryToGet.
                        RActor * ptRActor = RActorTryToGet(RActorID(ptACD->idOwner));
                        if (ptRActor)
                        {
                            VisualInventoryCopy(tACD.id, ptACD->id);
                        }
                    }
                }
            }
        }
    }
}




















