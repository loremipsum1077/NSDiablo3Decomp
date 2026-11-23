#include "UIToolTipControl.h"

#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../CGame.h"
#include "../UI.h"
#include "../UIInit.h"
#include "../../Lights.h"
#include "UIButtonControl.h"
#include "UIRActorControl.h"

// X/Client/Gfx/GfxCommon.cpp:54
extern const ImageTextureFrame cgtImageTextureFrameNull;
// X/Client/UserInterface/UI.cpp:2558
UIToolTip & UI_GetToolTipFromID(const UIID & id);
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);

// X/Client/UserInterface/UI.h:550; src/'s UI.h does not have this template yet.
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp
template <typename T>
inline void UI_DeleteControl(T *& ptControl)
{
    if (ptControl) {
        UIID id(ptControl->GetID());
        UI_DeleteControl(id, TRUE);
        ptControl = NULL;
    }
}
// X/Client/UserInterface/UI.cpp:4948
int32 UI_GetXMLInt32Constant(LPCSTR szConstantPath, int32 nDefaultValue);

#include "../../../Common/Items.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "UIItemButtonControl.h"
#include "UIConsoleScrollBoxControl.h"
#include "UIStackPanelControl.h"

// X/Client/UserInterface/UI.cpp:2503
UIItemButton & UI_GetItemButtonFromID(const UIID & id);
// X/Client/UserInterface/UI.cpp:2523
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
// X/Client/UserInterface/UI.cpp:2628
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);
// X/Common/Items.cpp:1858
BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem);
// X/Client/UserInterface/Interface/UIInventory.cpp:6343
namespace UIInventory { CRefString FormatItemComparisonField(const float flValue); }






static const uint32 TOOLTIP_FADE_TIME = 7;

static const uint32 TOOLTIP_FADE_IN_DELAY = 15;
static const uint32 TOOLTIP_FADE_IN_FADE_TIME = 7;
static const uint32 TOOLTIP_FADE_IN_TOTAL = TOOLTIP_FADE_IN_DELAY + TOOLTIP_FADE_IN_FADE_TIME;


static const float TEXT_GUTTER = 16.0f;


static int32 nRecipeUIDynamicIndex = 0;
static int32 nSkillUIDynamicIndex = 0;
static int32 nTraitUIDynamicIndex = 0;
static int32 nEnchantUIDynamicIndex = 0;
static int32 nItemUIDynamicIndex = 0;
static int32 nFriendUIDynamicIndex = 0;
static int32 nFriendItemUIDynamicIndex = 0;
static int32 nTitledUIDynamicIndex = 0;

static int32 nPlayerNameUIDynamicIndex = 0;
static int32 nAchievementUIDynamicIndex = 0;
static int32 nBnetSkillUIDynamicIndex = 0;
static int32 nInGamePortraitUIDynamicIndex = 0;
static int32 nBannerUIDynamicIndex = 0;
static int32 nArmorSetRewardUIDynamicIndex = 0;





RecipeTooltipData::RecipeTooltipData()
{

    gbidItem = -1;
    dwTooltipFlags = 0;
    eOverlay = 0;
    eIQL = -1;
    uszBaseStats = "";
    uszName = "";
    uszStats = "";
    uszType = "";
    uszRating = "";
    uszSetItem = "";
    tImageTexFrame = cgtImageTextureFrameNull;
    eTooltipType = -1;
}




















































































































































































































































































































































































































ItemTooltipData::ItemTooltipData()
{

    idACD = ACDID_INVALID;
    eTooltipType = -1;
    eItemEffectType = 0;
    gbid = -1;
    eIQL = -1;
    dwTooltipFlags = 0;
    eOverlay = 0;
    tImageTexFrame = cgtImageTextureFrameNull;
    uszName = NULL;
    uszType = NULL;
    uszRating = NULL;
    uszSpecialStats = NULL;
    uszClassReqs = NULL;
    uszRequirements = NULL;
    uszItemLevel = NULL;
    uszStats = NULL;
    uszSetItem = NULL;
    uszFlavor = NULL;
    uszTradableTo = NULL;
    uszCost = NULL;
    uszDurability = NULL;
    uszInstruction = NULL;
    uszSlot = NULL;
    uszSocketCost = NULL;
    uszRepairCost = NULL;
    uszTransmog = NULL;
    uszDye = NULL;
    fShowComparisonInfo = FALSE;
    flComparisonDeltaEHP = 0.0f;
    flComparisonDeltaDmg = 0.0f;
    flComparisonDeltaHealing = 0.0f;
    for (int32 i = 0; i < 3; i++) {
        uszSocketStats[i] = NULL;
        arrSocketGBIDs[i] = -1;
        arrSocketTextures[i] = cgtImageTextureFrameNull;
    }
}




















































TooltipUIElements::TooltipUIElements()

{
    m_ptWrapper = NULL;
}


TooltipUIElements::~TooltipUIElements()

{
    UI_DeleteControl(m_ptWrapper);
}




void TooltipUIElements::SetupWrapper(const SNO snoUI,
                                     LPCSTR szTemplate,
                                     LPCSTR szControlName,
                                     int32 & nDynamicIndex)
{

    UIControlSourceKey tTemplateSrcKey(snoUI, szTemplate);
    Vector2D vecOrigin(0.0f, 0.0f);
    m_ptWrapper = static_cast<UIWindow *>(UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("%s %d", szControlName, nDynamicIndex), cgUIIDTopLayer), &vecOrigin));
    m_ptWrapper->SetActive(FALSE);
    nDynamicIndex++;
}




void TooltipUIElements::HideControls()
{


    m_ptWrapper->SetActive(FALSE);
}




void TooltipUIElements::SetupSubControls(const UIID & id)

{
    m_ptWrapper->SwitchParent(id);
    m_ptWrapper->SetActive(TRUE);
}



EnchantUIElements::EnchantUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.enchantments", "enchantment", nEnchantUIDynamicIndex);
}






































BannerUIElements::BannerUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.banner", "banner", nBannerUIDynamicIndex);
}























































































ItemUIElements::ItemUIElements()
    : TooltipUIElements()
{
    m_idACD = ACDID_INVALID;

    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.items", "item", nItemUIDynamicIndex);
}




void ItemUIElements::SetupSubControlsData(
    const UIID & id,
    const ItemTooltipData & tData)
{
    m_idACD = tData.idACD;

    TooltipUIElements::SetupSubControls(id);
}




static void sSetTextureForItemQuality(
    UIWindow & tWnd,
    const ItemQualityLevel eIQL)
{
    switch (eIQL)
    {
    case IQL_INFERIOR:
    case IQL_NORMAL:
    case IQL_SUPERIOR:
        tWnd.SetTextureFrame("UI:Tooltip_Frame_QualityColor_Default");
        break;
    case IQL_LEGENDARY:
        tWnd.SetTextureFrame("UI:Tooltip_Frame_QualityColor_Legendary");
        break;
    case IQL_DISPLAY_AS_SET_ITEM:
        tWnd.SetTextureFrame("UI:Tooltip_Frame_QualityColor_Set");
        break;
    default:


        if (eIQL == IQL_SPECIAL || (eIQL >= IQL_MAGIC_AFFIX_START && eIQL <= IQL_MAGIC_AFFIX_END))
        {
            tWnd.SetTextureFrame("UI:Tooltip_Frame_QualityColor_Magic");
        }
        else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
        {
            tWnd.SetTextureFrame("UI:Tooltip_Frame_QualityColor_Rare");
        }
        else
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    }
}



void ItemUIElements::PositionSubControls(
    const ItemTooltipData & tData,
    const BOOL fEquipped,
    const BOOL fResetScrollPos)
{
    const UIID & idWrapper = m_ptWrapper->GetID();

    UIWindow & tWndEquippedSpacer = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.equipped_spacer"));
    tWndEquippedSpacer.SetActive(fEquipped);

    const BOOL bDisplayInstruction = tData.uszInstruction != NULL &&
        tData.uszInstruction[0] != '\0' &&
        !(tData.dwTooltipFlags & (1 << TOOLTIP_CREATE_CLOSE_BUTTON));


    UIWindow & tWndFrameEquipped = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".equipped_top_wrapper"));
    UIWindow & tWndFrameEquippedAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".equipped_top_wrapper_ancient"));
    UIWindow & tWndFrameEquippedPrimalAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".equipped_top_wrapper_primal"));
    UIWindow & tWndFrameTop = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.top_wrapper"));
    UIWindow & tWndFrameTopAncientFiligree = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.top_wrapper.ancient_filigree"));
    UIWindow & tWndFrameTopPrimalAncientFiligree = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.top_wrapper.primal_filigree"));
    UIWindow & tWndFrameBody = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame body"));
    UIWindow & tWndFrameCost = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_cost"));
    UIWindow & tWndFrameBottom = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_bottom"));
    UIWindow & tWndFrameBottomAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_bottom_ancient"));
    UIWindow & tWndFrameBottomPrimalAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_bottom_primal"));
    UIWindow & tWndFrameBottomAncientFiligree = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_bottom_ancient.ancient_filigree"));
    UIWindow & tWndFrameBottomPrimalAncientFiligree = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_bottom_primal.primal_filigree"));
    UIWindow & tWndFrameInstructionTopAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_instruction.ancient_top_instruction"));
    UIWindow & tWndFrameInstructionTopPrimalAncient = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_instruction.primal_top_instruction"));
    UIWindow & tWndFrameInstruction = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_instruction"));


    if (tData.dwTooltipFlags & (1 << TOOLTIP_IS_ANCIENT))
    {
        tWndFrameEquipped.SetActive(FALSE);
        tWndFrameEquippedAncient.SetActive(fEquipped);
        tWndFrameEquippedPrimalAncient.SetActive(FALSE);
        tWndFrameTopAncientFiligree.SetActive(TRUE);
        tWndFrameTopPrimalAncientFiligree.SetActive(FALSE);
        tWndFrameTop.SetTiledStyle(TILEDSTYLE_TOOLTIP_TITLE_ANCIENT);
        tWndFrameBody.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_BODY_ANCIENT);
        tWndFrameCost.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_COST_ANCIENT);
        tWndFrameBottom.SetActive(FALSE);
        tWndFrameBottomAncient.SetActive(TRUE);
        tWndFrameBottomPrimalAncient.SetActive(FALSE);
        tWndFrameBottomAncientFiligree.SetActive(TRUE);
        tWndFrameBottomPrimalAncientFiligree.SetActive(FALSE);
        tWndFrameInstructionTopAncient.SetActive(TRUE);
        tWndFrameInstructionTopPrimalAncient.SetActive(FALSE);
        tWndFrameInstruction.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_ANCIENT);
    }
    else if (tData.dwTooltipFlags & (1 << TOOLTIP_IS_PRIMAL_ANCIENT))
    {
        tWndFrameEquipped.SetActive(FALSE);
        tWndFrameEquippedAncient.SetActive(FALSE);
        tWndFrameEquippedPrimalAncient.SetActive(fEquipped);
        tWndFrameTopAncientFiligree.SetActive(FALSE);
        tWndFrameTopPrimalAncientFiligree.SetActive(TRUE);
        tWndFrameTop.SetTiledStyle(TILEDSTYLE_TOOLTIP_TITLE_PRIMAL_ANCIENT);
        tWndFrameBody.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_BODY_PRIMAL_ANCIENT);
        tWndFrameCost.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_COST_PRIMAL_ANCIENT);
        tWndFrameBottom.SetActive(FALSE);
        tWndFrameBottomAncient.SetActive(FALSE);
        tWndFrameBottomPrimalAncient.SetActive(TRUE);
        tWndFrameBottomAncientFiligree.SetActive(FALSE);
        tWndFrameBottomPrimalAncientFiligree.SetActive(TRUE);
        tWndFrameInstructionTopAncient.SetActive(FALSE);
        tWndFrameInstructionTopPrimalAncient.SetActive(TRUE);
        tWndFrameInstruction.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_PRIMAL_ANCIENT);
    }
    else
    {
        tWndFrameEquipped.SetActive(fEquipped);
        tWndFrameEquippedAncient.SetActive(FALSE);
        tWndFrameEquippedPrimalAncient.SetActive(FALSE);
        tWndFrameTopAncientFiligree.SetActive(FALSE);
        tWndFrameTopPrimalAncientFiligree.SetActive(FALSE);
        tWndFrameTop.SetTiledStyle(TILEDSTYLE_TOOLTIP_TITLE);
        tWndFrameBody.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_BODY);
        tWndFrameCost.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_COST);
        tWndFrameBottom.SetActive(TRUE);
        tWndFrameBottomAncient.SetActive(FALSE);
        tWndFrameBottomAncientFiligree.SetActive(FALSE);
        tWndFrameInstructionTopAncient.SetActive(FALSE);
        tWndFrameBottomPrimalAncient.SetActive(FALSE);
        tWndFrameBottomPrimalAncientFiligree.SetActive(FALSE);
        tWndFrameInstructionTopPrimalAncient.SetActive(FALSE);
        tWndFrameInstruction.SetTiledStyle(TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION);
    }

    {
        const UIID idName = UIIDCreateFromParent(idWrapper, ".stack.top_wrapper.stack.name");
        UIText & tNameText = UI_GetTextFromID(idName);
        if (tData.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
        {
            sSetTextureForItemQuality(tNameText, IQL_NORMAL);
        }
        else
        {
            sSetTextureForItemQuality(tNameText, tData.eIQL);
        }


        tNameText.SetText(tData.uszName, FALSE);
    }

    const UIID idContent = UIIDCreateFromParent(idWrapper, ".stack.frame body.scrollbox");

    UIWindow & tWndAccent = UI_GetWindowFromID(UIIDCreateFromParent(idContent, ".stack.main.accent"));
    CRefString sLabel;
    if (tData.eTooltipType != TOOLTIPTYPE_ITEM_OTHER)
    {
        if (tData.eTooltipType == TOOLTIPTYPE_ARMOR) sLabel = "UI:Tooltip_DPS_Armor";
        else
        {


            switch (tData.eItemEffectType)
            {
            case ITEM_EFFECT_TYPE_FIRE:
                sLabel = "UI:Tooltip_DPS_Elemental_Fire";
                break;
            case ITEM_EFFECT_TYPE_COLD:
                sLabel = "UI:Tooltip_DPS_Elemental_Ice";
                break;
            case ITEM_EFFECT_TYPE_LIGHTNING:
                sLabel = "UI:Tooltip_DPS_Elemental_Lightning";
                break;
            case ITEM_EFFECT_TYPE_POISON:
                sLabel = "UI:Tooltip_DPS_Elemental_Poison";
                break;
            case ITEM_EFFECT_TYPE_ARCANE:
                sLabel = "UI:Tooltip_DPS_Elemental_Arcane";
                break;
            case ITEM_EFFECT_TYPE_HOLY:
                sLabel = "UI:Tooltip_DPS_Elemental_Holy";
            }
        }
    }
    if (sLabel.Length())
    {
        tWndAccent.SetActive(TRUE);
        tWndAccent.SetTextureFrame(sLabel);
    }
    else
    {
        tWndAccent.SetActive(FALSE);
    }

    const BOOL fLargeItem = ItemIsDrawnAsLargeSlotItem(tData.gbid);
    const BOOL fCollectibleItem = ItemHasLabel(tData.gbid, ITL_COLLECTIBLE);

    UIWindow & tWndLargeIcon = UI_GetWindowFromID(UIIDCreateFromParent(idContent, ".stack.main.large_icon_frame"));
    UIItemButton & tLargeItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idContent, ".stack.main.large_itembutton"));
    tWndLargeIcon.SetActive(fLargeItem && !fCollectibleItem);
    tLargeItemButton.SetActive(fLargeItem && !fCollectibleItem);
    UIWindow & tWndSmallIcon = UI_GetWindowFromID(UIIDCreateFromParent(idContent, ".stack.main.small_icon_frame"));
    UIItemButton & tSmallItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idContent, ".stack.main.small_itembutton"));
    tWndSmallIcon.SetActive(!fLargeItem && !fCollectibleItem);
    tSmallItemButton.SetActive(!fLargeItem && !fCollectibleItem);
    UIWindow & tWndCollectibleIcon = UI_GetWindowFromID(UIIDCreateFromParent(idContent, ".stack.main.collectible_icon_frame"));
    UIItemButton & tCollectibleItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idContent, ".stack.main.collectible_itembutton"));
    tWndCollectibleIcon.SetActive(fCollectibleItem);
    tCollectibleItemButton.SetActive(fCollectibleItem);

    {
        UIItemButton * ptButton = fCollectibleItem ? &tCollectibleItemButton :
            fLargeItem ? &tLargeItemButton : &tSmallItemButton;

        ptButton->SetGBID(tData.gbid);
        ptButton->SetQualityLevel(tData.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH) ? IQL_NORMAL : tData.eIQL);
        ptButton->SetAllTextureFrames(tData.tImageTexFrame);
        ptButton->SetOverlay(tData.eOverlay);
    }

    UIText & tTextType = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.wrapper.col1.type"));
    tTextType.SetText(tData.uszType, FALSE);

    UIText & tTextSlot = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.wrapper.col2.slot"));
    if (tData.uszSlot != NULL &&
        tData.uszSlot[0] != '\0')
    {
        tTextSlot.SetActive(TRUE);
        tTextSlot.SetText(tData.uszSlot, FALSE);
    }
    else
    {
        tTextSlot.SetActive(FALSE);
    }

    UIText & tTextRating = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.wrapper.col1.stack.rating"));
    UIText & tTextRatingLabel = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.wrapper.col1.stack.rating_label"));
    if (tData.eTooltipType != TOOLTIPTYPE_ITEM_OTHER)
    {
        tTextRating.SetActive(TRUE);
        tTextRating.SetText(tData.uszRating, FALSE);
        tTextRating.SetDropShadowColor(crgbaBlack);

        tTextRatingLabel.SetActive(TRUE);
        if (tData.eTooltipType == TOOLTIPTYPE_ARMOR)
        {
            tTextRatingLabel.SetText(StringListFetch("H2OLayout:TooltipRatingLabelArmor"), FALSE);
        }
        else
        {
            tTextRatingLabel.SetText(StringListFetch("H2OLayout:TooltipRatingLabelDPS"), FALSE);
        }
    }
    else
    {
        tTextRating.SetActive(FALSE);
        tTextRatingLabel.SetActive(FALSE);
    }

    UIText & tTextClassReq = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.wrapper.col2.class_reqs"));
    if (tData.uszClassReqs != NULL &&
        tData.uszClassReqs[0] != '\0')
    {
        tTextClassReq.SetActive(TRUE);
        tTextClassReq.SetText(tData.uszClassReqs, FALSE);
    }
    else
    {
        tTextClassReq.SetActive(FALSE);
    }

    UIText & tTextSpecialStats = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.main.stack.special_stats"));
    if (tData.uszSpecialStats != NULL &&
        tData.uszSpecialStats[0] != '\0')
    {
        tTextSpecialStats.SetActive(TRUE);
        tTextSpecialStats.SetText(tData.uszSpecialStats, FALSE);
    }
    else
    {
        tTextSpecialStats.SetActive(FALSE);
    }

    UIText & tTextStats = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.stats"));
    if (tData.uszStats != NULL &&
        tData.uszStats[0] != '\0')
    {
        tTextStats.SetActive(TRUE);
        tTextStats.SetText(tData.uszStats, FALSE);
    }
    else
    {
        tTextStats.SetActive(FALSE);
    }

    UIText & tTextTransmog = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.transmog"));
    if (tData.uszTransmog != NULL &&
        tData.uszTransmog[0] != '\0')
    {
        tTextTransmog.SetActive(TRUE);
        tTextTransmog.SetText(tData.uszTransmog, FALSE);
    }
    else
    {
        tTextTransmog.SetActive(FALSE);
    }

    UIText & tTextDye = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.dye"));
    if (tData.uszDye != NULL &&
        tData.uszDye[0] != '\0')
    {
        tTextDye.SetActive(TRUE);
        tTextDye.SetText(tData.uszDye, FALSE);
    }
    else
    {
        tTextDye.SetActive(FALSE);
    }

    for (int32 i = 0; i < 3; i++)
    {
        UIWindow & tWndSocket = UI_GetWindowFromID(UIIDCreateFromParent(idContent, CRefString().Format(".stack.socket %d", i)));

        if (tData.uszSocketStats[i] == NULL || tData.uszSocketStats[i][0] == '\0')
        {
            tWndSocket.SetActive(FALSE);
            continue;
        }
        tWndSocket.SetActive(TRUE);

        UIItemButton & tItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(tWndSocket.GetID(), ".itembutton"));
        tItemButton.SetGBID(tData.arrSocketGBIDs[i]);
        tItemButton.SetQualityLevel(IQL_INVALID);
        tItemButton.SetAllTextureFrames(tData.arrSocketTextures[i]);

        UIText & tText = UI_GetTextFromID(UIIDCreateFromParent(tWndSocket.GetID(), ".text"));
        tText.SetText(tData.uszSocketStats[i], FALSE);
    }

    UIText & tTextSetItem = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.set_item"));
    if (tData.uszSetItem != NULL &&
        tData.uszSetItem[0] != '\0')
    {
        tTextSetItem.SetActive(TRUE);
        tTextSetItem.SetText(tData.uszSetItem, FALSE);
    }
    else
    {
        tTextSetItem.SetActive(FALSE);
    }


    UIWindow & tWndComparison = UI_GetWindowFromID(UIIDCreateFromParent(idContent, ".stack.comparison"));
    if (!fEquipped &&
        tData.fShowComparisonInfo)
    {
        tWndComparison.SetActive(TRUE);

        CRefString strEHP;
        CRefString strDmg;
        CRefString strHealing;
        if (tData.eOverlay == ITEMOVERLAY_UNIDENTIFIED)
        {
            CRefString sMysteryVal = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SHOP), "MysteryValue");
            strEHP = sMysteryVal;
            strDmg = sMysteryVal;
            strHealing = sMysteryVal;
        }
        else
        {
            strDmg = UIInventory::FormatItemComparisonField(tData.flComparisonDeltaDmg);
            strEHP = UIInventory::FormatItemComparisonField(tData.flComparisonDeltaEHP);
            strHealing = UIInventory::FormatItemComparisonField(tData.flComparisonDeltaHealing);
        }

        CRefString strDmgTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ComparisonDamage");
        strDmg += " ";
        strDmg += strDmgTitle;

        CRefString strEHPTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ComparisonEHP");
        strEHP += " ";
        strEHP += strEHPTitle;

        CRefString strHealingTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ComparisonHealing");
        strHealing += " ";
        strHealing += strHealingTitle;

        UIText & tTxtDmgComparison = UI_GetTextFromID(UIIDCreateFromParent(tWndComparison.GetID(), ".stack.field_0.valuename"));
        UIText & tTxtEHPComparison = UI_GetTextFromID(UIIDCreateFromParent(tWndComparison.GetID(), ".stack.field_1.valuename"));
        UIText & tTxtHealingComparison = UI_GetTextFromID(UIIDCreateFromParent(tWndComparison.GetID(), ".stack.field_2.valuename"));
        tTxtDmgComparison.SetText(strDmg.str(), FALSE);
        tTxtEHPComparison.SetText(strEHP.str(), FALSE);
        tTxtHealingComparison.SetText(strHealing.str(), FALSE);
    }
    else
    {
        tWndComparison.SetActive(FALSE);
    }

    UIText & tTextFlavor = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.flavor"));
    tTextFlavor.SetText(tData.uszFlavor, FALSE);

    UIText & tTextItemLevel = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.wrapper.itemLevel"));
    if (tData.uszItemLevel != NULL &&
        tData.uszItemLevel[0] != '\0')
    {
        tTextItemLevel.SetActive(TRUE);
        tTextItemLevel.SetText(tData.uszItemLevel, FALSE);
    }
    else
    {
        tTextItemLevel.SetActive(FALSE);
    }

    UIText & tTextReqs = UI_GetTextFromID(UIIDCreateFromParent(idContent, ".stack.wrapper.reqs"));
    if (tData.uszRequirements != NULL &&
        tData.uszRequirements[0] != '\0')
    {
        tTextReqs.SetActive(TRUE);
        tTextReqs.SetText(tData.uszRequirements, FALSE);
    }
    else
    {
        tTextReqs.SetActive(FALSE);
    }

    const BOOL fHasCost = tData.uszCost != NULL && tData.uszCost[0] != '\0';
    const BOOL fHasDurability = tData.uszDurability != NULL && tData.uszDurability[0] != '\0';
    const BOOL fHasSocketCost = tData.uszSocketCost != NULL && tData.uszSocketCost[0] != '\0';
    const BOOL fHasRepairCost = tData.uszRepairCost != NULL && tData.uszRepairCost[0] != '\0';

    if (fHasCost || fHasDurability || fHasSocketCost || fHasRepairCost)
    {



        tWndFrameCost.SetActive(TRUE);

        UIText & tTextCost = UI_GetTextFromID(UIIDCreateFromParent(tWndFrameCost.GetID(), ".cost"));
        tTextCost.SetText(tData.uszCost, FALSE);
        UIText & tTextDurability = UI_GetTextFromID(UIIDCreateFromParent(tWndFrameCost.GetID(), ".durability"));
        tTextDurability.SetText(tData.uszDurability, FALSE);

        UIText & tTextSocketCost = UI_GetTextFromID(UIIDCreateFromParent(tWndFrameCost.GetID(), ".socket_cost"));
        if (fHasSocketCost || fHasRepairCost)
        {
            tTextSocketCost.SetActive(TRUE);
            tTextSocketCost.SetText(fHasSocketCost ? tData.uszSocketCost : tData.uszRepairCost, FALSE);
        }
        else
        {
            tTextSocketCost.SetActive(FALSE);
        }
    }
    else
    {
        tWndFrameCost.SetActive(FALSE);
    }

    if (bDisplayInstruction)
    {
        tWndFrameInstruction.SetActive(TRUE);

        UIText & tTextInstruction = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.frame_instruction.stack.text"));
        tTextInstruction.SetText(tData.uszInstruction, FALSE);
    }
    else
    {
        tWndFrameInstruction.SetActive(FALSE);
    }



    UIConsoleScrollBox & tScrollBox = UI_GetConsoleScrollBoxFromID(UIIDCreateFromParent(idWrapper, ".stack.frame body.scrollbox"));
    tScrollBox.SetExternallyControlled(FALSE);

    if (fResetScrollPos)
    {
        tScrollBox.SetScrollPos(0.0f);
    }

    Vector2D vecScrollBoxTL, vecScrollBoxDim;
    tScrollBox.GetRelativePosition(vecScrollBoxTL, vecScrollBoxDim);

    UIStackPanel & tInnerStack = UI_GetStackPanelFromID(UIIDCreateFromParent(idWrapper, ".stack.frame body.scrollbox.stack"));
    Vector2D vecInnerTL, vecInnerDim;
    tInnerStack.GetRelativePosition(vecInnerTL, vecInnerDim);
    vecScrollBoxDim.y = vecInnerDim.y;
    tScrollBox.SetRelativePosition(vecScrollBoxTL, vecScrollBoxDim);

    Vector2D vecWrapperTL, vecWrapperDim;
    m_ptWrapper->GetRelativePosition(vecWrapperTL, vecWrapperDim);
    const float flFrameHeight = vecWrapperDim.y - vecScrollBoxDim.y;

    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);
    rectViewportUIC.bottom -= flFrameHeight;

    vecScrollBoxDim.y = Math::Min(rectViewportUIC.Height(), vecScrollBoxDim.y);
    tScrollBox.SetRelativePosition(vecScrollBoxTL, vecScrollBoxDim);
}












RecipeUIElements::RecipeUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.recipes", "recipe", nRecipeUIDynamicIndex);
}


// Dead-stripped from the binary (DW_AT_low_pc 0); kept because it is the second caller of
// sSetTextureForItemQuality. Locals, their lines and the inlined calls (GetID 1330, SetOverlay 1377, SetDropShadowColor 1413) are from the DWARF; the statements are inferred from ItemUIElements::PositionSubControls.
void RecipeUIElements::PositionSubControls(
    const RecipeTooltipData & tData)
{
    const UIID & idWrapper = m_ptWrapper->GetID();

    {
        const UIID idName = UIIDCreateFromParent(idWrapper, ".stack.top_wrapper.stack.name");
        UIText & tNameText = UI_GetTextFromID(idName);
        sSetTextureForItemQuality(tNameText, tData.eIQL);
        tNameText.SetText(tData.uszName, FALSE);
    }



    UIWindow & tWndAccent = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.main.accent"));
    CRefString sLabel;
    if (tData.eTooltipType == TOOLTIPTYPE_ARMOR)
    {
        sLabel = "UI:Tooltip_DPS_Armor";
    }

    if (sLabel.Length())
    {
        tWndAccent.SetActive(TRUE);
        tWndAccent.SetTextureFrame(sLabel);
    }
    else
    {
        tWndAccent.SetActive(FALSE);
    }




    const BOOL fLargeItem = ItemIsDrawnAsLargeSlotItem(tData.gbidItem);

    UIWindow & tWndLargeIcon = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.main.large_icon_frame"));
    UIItemButton & tLargeItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idWrapper, ".stack.main.large_itembutton"));
    tWndLargeIcon.SetActive(fLargeItem);
    tLargeItemButton.SetActive(fLargeItem);
    UIWindow & tWndSmallIcon = UI_GetWindowFromID(UIIDCreateFromParent(idWrapper, ".stack.main.small_icon_frame"));
    UIItemButton & tSmallItemButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idWrapper, ".stack.main.small_itembutton"));
    tWndSmallIcon.SetActive(!fLargeItem);
    tSmallItemButton.SetActive(!fLargeItem);

    {
        UIItemButton * ptButton = fLargeItem ? &tLargeItemButton : &tSmallItemButton;
        ptButton->SetGBID(tData.gbidItem);
        ptButton->SetQualityLevel(tData.eIQL);
        ptButton->SetAllTextureFrames(tData.tImageTexFrame);
        ptButton->SetOverlay(tData.eOverlay);
    }

    UIText & tTextType = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.wrapper.col1.type"));
    tTextType.SetText(tData.uszType, FALSE);

    UIText & tTextSlot = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.wrapper.col2.slot"));
    if (tData.uszSlot.Length())

    {
        tTextSlot.SetActive(TRUE);
        tTextSlot.SetText(tData.uszSlot, FALSE);
    }
    else
    {
        tTextSlot.SetActive(FALSE);
    }

    UIText & tTextClassReq = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.wrapper.col2.class_reqs"));
    if (tData.uszClassReqs.Length())

    {
        tTextClassReq.SetActive(TRUE);
        tTextClassReq.SetText(tData.uszClassReqs, FALSE);
    }
    else
    {
        tTextClassReq.SetActive(FALSE);
    }

    UIText & tTextRating = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.wrapper.col1.stack.rating"));
    UIText & tTextRatingLabel = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.wrapper.col1.stack.rating_label"));
    if (tData.eTooltipType != TOOLTIPTYPE_ITEM_OTHER)
    {
        tTextRating.SetActive(TRUE);
        tTextRating.SetText(tData.uszRating, FALSE);
        tTextRating.SetDropShadowColor(crgbaBlack);

        tTextRatingLabel.SetActive(TRUE);
        if (tData.eTooltipType == TOOLTIPTYPE_ARMOR)
        {
            tTextRatingLabel.SetText(StringListFetch("H2OLayout:TooltipRatingLabelArmor"), FALSE);
        }
        else
        {
            tTextRatingLabel.SetText(StringListFetch("H2OLayout:TooltipRatingLabelDPS"), FALSE);
        }
    }
    else
    {
        tTextRating.SetActive(FALSE);
        tTextRatingLabel.SetActive(FALSE);
    }

    UIText & tTextSpecialStats = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.main.stack.special_stats"));
    if (tData.uszBaseStats.Length())

    {
        tTextSpecialStats.SetActive(TRUE);
        tTextSpecialStats.SetText(tData.uszBaseStats, FALSE);
    }
    else
    {
        tTextSpecialStats.SetActive(FALSE);
    }

    UIText & tTextStats = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.stats"));
    if (tData.uszStats.Length())

    {
        tTextStats.SetActive(TRUE);
        tTextStats.SetText(tData.uszStats, FALSE);
    }
    else
    {
        tTextStats.SetActive(FALSE);
    }

    UIText & tTextSetItem = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.set_item"));
    if (tData.uszSetItem.Length())

    {
        tTextSetItem.SetActive(TRUE);
        tTextSetItem.SetText(tData.uszSetItem, FALSE);
    }
    else
    {
        tTextSetItem.SetActive(FALSE);
    }

    UIText & tTextItemLevel = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.wrapper.itemLevel"));
    if (tData.uszItemLevel.Length())

    {
        tTextItemLevel.SetActive(TRUE);
        tTextItemLevel.SetText(tData.uszItemLevel, FALSE);
    }
    else
    {
        tTextItemLevel.SetActive(FALSE);
    }

    UIText & tTextReqs = UI_GetTextFromID(UIIDCreateFromParent(idWrapper, ".stack.wrapper.reqs"));
    if (tData.uszRequirements.Length())

    {
        tTextReqs.SetActive(TRUE);
        tTextReqs.SetText(tData.uszRequirements, FALSE);
    }
    else
    {
        tTextReqs.SetActive(FALSE);
    }
}



SkillUIElements::SkillUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.skills", "skill", nSkillUIDynamicIndex);
}















































































































































TraitUIElements::TraitUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "Tooltip");
    SetupWrapper(snoname.snoHandle, "tooltip_dialog_background.traits", "trait", nTraitUIDynamicIndex);
}









































TitledUIElements::TitledUIElements()
    : TooltipUIElements()
{
}















































































































PlayerNameUIElements::PlayerNameUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "BattleNetPlayerTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetPlayerTooltip_main.LayoutRoot.Tooltip", "player name", nPlayerNameUIDynamicIndex);
}




void PlayerNameUIElements::HideControls()
{

    UIID idPlayerSigil;
    idPlayerSigil.SetFromParent(m_ptWrapper->GetID(), "Sigil");

    if (UI_TestID(idPlayerSigil)) {

        UIRActor & tRActor = UI_GetRActorFromID(idPlayerSigil);
        tRActor.DeleteRActor();
    }

    TooltipUIElements::HideControls();
}

















































































AchievementUIElements::AchievementUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "BattleNetAchievementsTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetAchievementsTooltip_main.LayoutRoot.AchievementsTooltip", "Achievement tooltip", nAchievementUIDynamicIndex);
}

























































































































































































BnetSkillUIElements::BnetSkillUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "BattleNetProfileSkillTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetProfileSkillTooltip_main.LayoutRoot.SkillTooltip", "skill tooltip", nBnetSkillUIDynamicIndex);
}



















































InGamePortraitUIElements::InGamePortraitUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "BattleNetInGamePortraitTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetInGamePortraitTooltip_main.LayoutRoot.PortraitTooltip", "in game portrait tooltip", nInGamePortraitUIDynamicIndex);
}





















































RewardUIElements::RewardUIElements()
    : TooltipUIElements()
{
    SNOName snoname(UI_SNO, "BattleNetAchievementsRewardTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetAchievementsRewardTooltip_main.LayoutRoot.AchievementRewardTooltip", "reward tooltip", nBnetSkillUIDynamicIndex);
}









































































































FriendInviteUIElements::FriendInviteUIElements()

{
    SNOName snoname(UI_SNO, "BattleNetFriendsListFriendRequestTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetFriendsListFriendRequestTooltip_main.LayoutRoot.FriendRequestTooltip", "friend invite tooltip", nBnetSkillUIDynamicIndex);
}






































BnetPlayerUIElements::BnetPlayerUIElements()

{
    SNOName snoname(UI_SNO, "BattleNetFriendsListPlayerTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetFriendsListPlayerTooltip_main.LayoutRoot.PlayerTooltip", "bnet player tooltip", nBnetSkillUIDynamicIndex);
}















































































FriendUIElements::FriendUIElements()
    : TooltipUIElements()
    , m_heroModelDescription()
    , m_idLight(-1)
{
    SNOName snoname(UI_SNO, "PlayerTooltip");
    SetupWrapper(snoname.snoHandle, "PlayerTooltip_main.LayoutRoot", "friend", nFriendUIDynamicIndex);

    UIWindow & tBody = UI_GetWindowFromID(UIIDCreateFromParent(m_ptWrapper->GetID(), ".Body"));
    tBody.GetRelativePosition(m_vecBodyTextureTL, m_vecBodyTextureDim);

    UIWindow & tBottom = UI_GetWindowFromID(UIIDCreateFromParent(m_ptWrapper->GetID(), ".Bottom"));
    tBottom.GetRelativePosition(m_vecBottomContainerTL, m_vecBottomContainerDim);

    UIWindow & tPartyContainer = UI_GetWindowFromID(UIIDCreateFromParent(m_ptWrapper->GetID(), ".Bottom.PartyContainer"));
    tPartyContainer.GetRelativePosition(m_vecPartyContainerTL, m_vecPartyContainerDim);
}




void FriendUIElements::HideControls()
{

    TooltipUIElements::HideControls();

    UIID idHeroModelRactor = UIIDCreateFromParent(m_ptWrapper->GetID(), ".Bottom.HeroContainer.Hero3DModel");
    if (UI_TestID(idHeroModelRactor)) {

        UIRActor & tPlayerModel = UI_GetRActorFromID(idHeroModelRactor);
        tPlayerModel.SetActive(FALSE);
        tPlayerModel.DeleteRActor();
        tPlayerModel.Cleanup();
    }

    if (m_idLight != -1) {

        LightDelete(m_idLight);
        m_idLight = -1;
    }
}
















































































































































































































































































































ArmorSetRewardUIElements::ArmorSetRewardUIElements()

{
    SNOName snoname(UI_SNO, "BattleNetArmorSetRewardTooltip");
    SetupWrapper(snoname.snoHandle, "BattleNetArmorSetRewardTooltip_main.LayoutRoot.TooltipWrapper", "ArmorSetReward tooltip", nArmorSetRewardUIDynamicIndex);
}










































































void UIToolTip::sInit()
{

    m_idActiveOwner = GetUIIDNone();
    m_dwFadeStart = 0;
    m_dwFadeInStart = 0;
    m_bAlpha = 255;
    m_rectParent = cgrectNull;

    m_dwFlags |= (1 << UIWINDOW_IGNORES_CLIPPING_BIT);

    m_eTooltipType = TOOLTIPTYPE_UNKNOWN;

    m_ptTextSubControl = NULL;
    m_bDirty = FALSE;
    m_dwNumSocketedRunes = 0;
    m_nInvestedPoints = 0;
    m_dwTooltipFlags = 0;
    m_ptCloseButtonSubControl = NULL;
}
















UIToolTip::UIToolTip(
    const WindowCreationParams & tParams,
    const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    sInit();
}




UIToolTip::~UIToolTip()

{
    sDisposeSubControlsForType();
}




void UIToolTip::sDisposeSubControlsForType()
{

    if (m_eTooltipType == TOOLTIPTYPE_TEXT)
    {

        UI_DeleteControl(m_ptTextSubControl);
    }


    UI_DeleteControl(m_ptCloseButtonSubControl);
}




void UIToolTip::sHideSubControlsForType()

{
    switch (m_eTooltipType)
    {
    case TOOLTIPTYPE_TEXT:
        break;

    case TOOLTIPTYPE_ARMOR:
    case TOOLTIPTYPE_WEAPON:
    case TOOLTIPTYPE_ITEM_OTHER:
        m_tItemUI.HideControls();
        break;

    case TOOLTIPTYPE_SKILL:
        m_tSkillUI.HideControls();
        break;

    case TOOLTIPTYPE_TRAIT:
        m_tTraitUI.HideControls();
        break;

    case TOOLTIPTYPE_RECIPE:
        m_tRecipeUI.HideControls();
        break;

    case TOOLTIPTYPE_ENCHANT:
        m_tEnchantUI.HideControls();
        break;

    case TOOLTIPTYPE_TITLED:
        m_tTitledUI.HideControls();
        break;

    case TOOLTIPTYPE_BANNER:
        m_tBannerUI.HideControls();
        break;

    case TOOLTIPTYPE_PLAYERNAME:
        m_tPlayerNameUI.HideControls();
        break;

    case TOOLTIPTYPE_ACHIEVEMENT:
        m_tAchievementUI.HideControls();
        break;

    case TOOLTIPTYPE_BNETSKILL:
        m_tBnetSkillUI.HideControls();
        break;

    case TOOLTIPTYPE_REWARD:
        m_tRewardUI.HideControls();
        break;

    case TOOLTIPTYPE_FRIENDINVITE:
        m_tFriendInviteUI.HideControls();
        break;

    case TOOLTIPTYPE_BNETPLAYER:
        m_tBnetPlayerUI.HideControls();
        break;

    case TOOLTIPTYPE_INGAMEPORTRAIT:
        m_tInGamePortraitUI.HideControls();
        break;

    case TOOLTIPTYPE_FRIEND:
        m_tFriendUI.HideControls();
        break;

    case TOOLTIPTYPE_ARMORSETREWARD:
        m_tArmorSetRewardUI.HideControls();
        break;
    }
}




BOOL UIToolTip::sChangeTooltipType(
    const UIID & idParent,
    const TooltipType eNewTooltipType)
{
    SetParentID(idParent);

    if (m_eTooltipType == eNewTooltipType &&
        !m_bDirty)
    {
        return FALSE;
    }

    sDisposeSubControlsForType();
    sHideSubControlsForType();
    m_eTooltipType = eNewTooltipType;
    m_bDirty = FALSE;
    return TRUE;
}




void UIToolTip::sSetupTextSubControls(
    const UIJustifyHorizontal eJustifyHorizontal)
{
    const UIID & id = GetID();

    SNOName snoname(UI_SNO, "Tooltip");
    UIControlSourceKey tTemplateSrcKey(snoname.snoHandle, "tooltip_dialog_background.text_tooltip");
    Vector2D vecOrigin(0.0f, 0.0f);
    UIControl * ptControl = UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams("tooltip", id), &vecOrigin);
    m_ptTextSubControl = &UI_GetTextFromID(ptControl->GetID());

    m_ptTextSubControl->SetJustification(eJustifyHorizontal, UIJV_TOP);

    SetTiledStyle(TILEDSTYLE_TEXT_TOOLTIP);
}




void UIToolTip::SetTextTooltipData(
    const UIID & idParent,
    LPCSTR uszTip,
    const UIJustifyHorizontal eJustifyHorizontal,
    const BOOL fForceRefresh)
{
    if (fForceRefresh)
    {
        m_bDirty = TRUE;
    }
    if (sChangeTooltipType(idParent, TOOLTIPTYPE_TEXT))
    {
        sSetupTextSubControls(eJustifyHorizontal);
    }

    int32 nTipWidth = -1;
    UIText & tText = *m_ptTextSubControl;


    if (m_idActiveOwner != GetUIIDNone())
    {
        UIWindow & tParent = UI_GetWindowFromID(m_idActiveOwner);
        nTipWidth = tParent.GetTextTooltipWidth();
    }

    Vector2D vecTextInsetUIC(TEXT_GUTTER, TEXT_GUTTER);
    tText.SetTextInset(vecTextInsetUIC);

    if (nTipWidth == -1)
    {

        tText.SetWordWrap(FALSE);
    }
    else
    {
        tText.SetRelativePosition(Vector2D(0.0f, 0.0f), Vector2D((float)nTipWidth, 0.0f));
        tText.SetWordWrap(TRUE);
    }
    tText.SetText(uszTip, FALSE);


    Vector2D vecTextSizeUIC;
    tText.GetTextRenderedSizeUIC(vecTextSizeUIC, FALSE);


    vecTextSizeUIC.x += vecTextInsetUIC.x * 2.0f + 2.0f;
    vecTextSizeUIC.y += vecTextInsetUIC.y * 2.0f + 2.0f;

    tText.SetRelativePosition(Vector2D(0.0f, 0.0f), vecTextSizeUIC);


    SetRelativePosition(Vector2D(0.0f, 0.0f), vecTextSizeUIC);
}




void UIToolTip::SetupCloseButton()
{

    const UIID & id = GetID();

    SNOName snoname(UI_SNO, "Tooltip");
    UIControlSourceKey tTemplateSrcKey(snoname.snoHandle, "tooltip_dialog_background.button_close");
    UIControl * ptControl = UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams("close button", id), NULL);
    m_ptCloseButtonSubControl = &UI_GetButtonFromID(ptControl->GetID());
}




void UIToolTip::PositionCloseButton()
{


    if (m_ptCloseButtonSubControl)
    {
        const int32 nCloseButtonOffsetX = UI_GetXMLInt32Constant("ToolTip:c_CloseButtonOffsetX", -4);


        const Vector2D vecOffsetUIC((float)nCloseButtonOffsetX, 0.0f);

        Vector2D vecTipTL, vecTipDim;
        GetRelativePosition(vecTipTL, vecTipDim);

        Vector2D vecTL, vecDim;
        m_ptCloseButtonSubControl->GetRelativePosition(vecTL, vecDim);
        vecTL.Set(vecTipDim.x + vecOffsetUIC.x, vecTL.y);
        m_ptCloseButtonSubControl->SetRelativePosition(vecTL, vecDim);
    }
}



void UIToolTip::SetItemTooltipData(
    const UIID & idParent,
    const ItemTooltipData & tData,
    const BOOL bResetScrollPos)
{
    m_dwTooltipFlags = tData.dwTooltipFlags;

    const BOOL fEquipped =
        ((tData.dwTooltipFlags >> TOOLTIP_CURRENTLY_EQUIPPED_PLAYER_BIT) & 1) | ((tData.dwTooltipFlags >> TOOLTIP_CURRENTLY_EQUIPPED_HIRELING_BIT) & 1);
















    if (sChangeTooltipType(idParent, tData.eTooltipType))
    {
        const UIID & id = GetID();
        m_tItemUI.SetupSubControlsData(id, tData);

        if (tData.dwTooltipFlags & (1 << TOOLTIP_CREATE_CLOSE_BUTTON))
        {
            SetupCloseButton();
        }

        SetTiledStyle(TILEDSTYLE_NONE);
    }

    m_dwFlags |= (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT);
    m_tItemUI.PositionSubControls(tData, fEquipped, bResetScrollPos);

    PositionCloseButton();
}


















































































































































































































































































































































void UIToolTip::SetParentID(
    const UIID & idParent)
{

    if (idParent != m_idActiveOwner &&
        idParent != GetUIIDNone())
    {
        m_bDirty = TRUE;
    }

    m_idActiveOwner = idParent;
}

































































void UIToolTip::SetActive(
    const BOOL fActive)
{
    if (fActive)
    {

        m_dwFadeStart = 0;
    }
    else
    {
        m_dwFadeInStart = 0;
    }


    if (IsActive() == fActive)
    {
        return;
    }
    if (m_ptCloseButtonSubControl)
    {
        UIWindow::SetActive(fActive);
        return;
    }

    if (fActive) { UIWindow::SetActive(fActive); return; }

    m_idActiveOwner = GetUIIDNone();
    m_dwFadeStart = CGameGetRealTicks();
    m_bDirty = TRUE;





}



















































UIID UIToolTip::HitTest(
    const IVector2D & pt)
{
    if (m_ptCloseButtonSubControl)
    {
        const UIID & idHit = m_ptCloseButtonSubControl->HitTest(pt);
        if (idHit != GetUIIDNone())
        {
            return idHit;
        }


        if (IsInside(pt))
        {
            return GetID();
        }
    }

    return GetUIIDNone();
}



void UIToolTip::Draw(
    const uint8,
    float & flZSlice)
{
    if (m_bAlpha == 0)
    {
        return;
    }

    UIWindow::Draw(m_bAlpha, flZSlice);
}



void UIToolTip::OnHyperlinkClose(const UIID & id)
{
    UIButton & tButton = UI_GetButtonFromID(id);
    const UIID & idParent = tButton.GetParent();
    UIToolTip & tTip = UI_GetToolTipFromID(idParent);
    tTip.SetActive(FALSE);
}



// Dead-stripped (DW_AT_low_pc 0, no locals). Body inferred: nFriendUIDynamicIndex sits in the merged-globals block at +0x28 (DWARF DW_AT_location), which
// needs a function that uses it together with the other counters; resetting the counters per game is that use. With it the Friend and UIToolTip ctors match.
void UIToolTip::InitializeForGame()
{
    nRecipeUIDynamicIndex = 0;
    nSkillUIDynamicIndex = 0;
    nTraitUIDynamicIndex = 0;
    nEnchantUIDynamicIndex = 0;
    nItemUIDynamicIndex = 0;
    nFriendUIDynamicIndex = 0;
    nFriendItemUIDynamicIndex = 0;
    nTitledUIDynamicIndex = 0;
    nPlayerNameUIDynamicIndex = 0;
    nAchievementUIDynamicIndex = 0;
    nBnetSkillUIDynamicIndex = 0;
    nInGamePortraitUIDynamicIndex = 0;
    nBannerUIDynamicIndex = 0;
    nArmorSetRewardUIDynamicIndex = 0;
}





void UIToolTip::StartFadeInTimer()
{
    m_dwFadeInStart = CGameGetRealTicks();
}
