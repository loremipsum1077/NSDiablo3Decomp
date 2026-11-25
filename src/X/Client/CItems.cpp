// Original file: X/Client/CItems.cpp
#include <limits.h>
#include "./CGame.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Items.h"
#include "../Common/Player.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "./UserInterface/Controls/UITextControl.h"
#include "../Common/UserInterface/UITextControl.h"
#include "../Sigma/Unicode.h"
#include "../Sigma/RefString.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Definitions/PowerDefinition.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Common/TagMaps/PowerTags.h"
#include "../Common/UserInterface/UIToolTips.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/Definitions/StringListDefinition.h"

// Local declarations; signatures from the DWARF of the callers in this CU.
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  // X/Common/Items.cpp:1883
CRefString GetAttributeDescription(const AttribTooltipInfo & tInfo, const ActorCommonData & tACDItem, const AttribGroupID idFastAttrib, const PlayerClass ePlayerClass, ToolTipValueColoring eColoringType, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);  // X/Common/Items.cpp:11606
extern const int32 cgNumTooltipAttribsPrimary;  // X/Common/Items.cpp:501
extern const int32 cgNumTooltipAttribsSecondary;  // X/Common/Items.cpp:628
extern const int32 cgNumTooltipAttribsLegendary;  // X/Common/Items.cpp:640
extern const int32 cgNumTooltipAttribsTrailing;  // X/Common/Items.cpp:688
extern const AttribTooltipInfo ctAttribTooltipInfoPrimary[117];  // X/Common/Items.cpp:216
extern const AttribTooltipInfo ctAttribTooltipInfoSecondary[122];  // X/Common/Items.cpp:392
extern const AttribTooltipInfo ctAttribTooltipInfoLegendary[7];  // X/Common/Items.cpp:597
extern const AttribTooltipInfo ctAttribTooltipInfoTrailing[43];  // X/Common/Items.cpp:611
BOOL ItemIsRandomItemPlaceholder(const GBID gbidItem);  // X/Common/Items.cpp:2342

uint32 ItemGetRemainingCooldownTime(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);  // X/Common/Items.cpp
uint32 ItemGetCooldownTimeToApply(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:2488
uint32 ItemGetTradeTimeLeft(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:10344
ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor);  // X/Common/Items.cpp:3963
CRefString StringListEntryReplace(LPCSTR szString, LPCSTR szToken, LPCSTR szReplacement, BOOL bReplaceAll);  // X/Common/Definitions/StringListDefinition.cpp












static const int32 MAX_AFFIX_ATTRIBUTES = 4;  // GameBalanceDefinition.h:1377
namespace UISkills { BOOL ResolvePowerTag(const ActorCommonData * ptACD, AttribGroupID idFastAttrib, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString &, SNO snoPower, int32 nLevel, void *, void *, FontSpecifier &); }  // X/Common/UserInterface/UISkillsCommon.cpp:32
void FastAttribLightCopy(AttribGroupID idAttribGroupSource, AttribGroupID idAttribGroupDest);  // X/Common/Attributes/FastAttribute.cpp:4615















//@ 70
struct SpellRuneData {  // size 0x10
    LPCSTR pszName;  // 0x00
    Attrib eAttrib;  // 0x08
};

//@ 76
static const SpellRuneData ctSpellRuneData[5] =
{
    { "Rune_A", ATTRIBUTE_RUNE_A },
    { "Rune_B", ATTRIBUTE_RUNE_B },
    { "Rune_C", ATTRIBUTE_RUNE_C },
    { "Rune_D", ATTRIBUTE_RUNE_D },
    { "Rune_E", ATTRIBUTE_RUNE_E },
};
//@ 85
static const int32 scgNumSpellRuneData = sizeof(ctSpellRuneData) / sizeof(ctSpellRuneData[0]);



//@ 90
RGBAColor ItemGetCraftingMatDisplayNameColor(
    GBID gbidItem)
{




    RGBAColor rgbaColor;
    if (ItemHasForcedDisplayNameColor(gbidItem, rgbaColor))
    {
        return rgbaColor;
    }

    ItemQualityLevel eIQL = IQL_NORMAL;
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem != NULL && (eIQL = pdefItem->eDisplayedQualityLevel) == IQL_INVALID)
    {
        eIQL = IQL_NORMAL;
    }

    return ItemGetDisplayNameColor(eIQL);
}




//@ 117
UIItemOverlay ItemGetOverlay(
    const ActorCommonData & tACDItem,
    const ActorCommonData & tACDPlayer)
{
    if (ItemIsRandomItemPlaceholder(tACDItem.hGB.gbid))
    {
        PlayerClass eClass = PlayerGetPlayerClass(&tACDPlayer);
        return ItemMeetsOwnerClassRestrictions(tACDItem.hGB.gbid, eClass, FALSE) ? 4 : 2;
    }







    if (ItemIsUnidentified(tACDItem))
    {
        return 3;
    }

    {
        GameError eError;
        if (ItemOwnerMeetsRequirements(tACDItem, tACDPlayer, &eError))
        {
            return 0;
        }
        return eError == 30 ? 2 : 1;
    }
}












static BOOL sResolveSpellRuneTag(
    const ActorCommonData * ptACD,
    AttribGroupID idFastAttrib,
    AttribGroupID,
    LPCSTR szTag,
    float & flResult,
    float *,
    CRefString &,
    SNO snoPower,
    int32 nLevel,
    void * pData,
    void *,
    FontSpecifier & eFontColor)
{
    BOOL bMatch = FALSE;
    BOOL bDelta = FALSE;
    const char * szTagStart = szTag;
    while (TRUE)
    {
        char cChar = *szTag;
        if (cChar == '~')
        {
            bDelta = TRUE;
        }
        else if (cChar != ' ')
        {
            break;
        }

        szTag++;
    }

    Attrib eAttrib = (Attrib)(int64)pData;
    FastAttribKey tKey = MakeAttribKey(eAttrib, snoPower);
    if (snoPower != -1)
    {
        float flResultBefore = 0.0f;
        float flResultAfter = 0.0f;
        CRefString strResult;
        bMatch = UISkills::ResolvePowerTag(ptACD, idFastAttrib, -1, szTag, flResultBefore, NULL, strResult, snoPower, nLevel, pData, NULL, eFontColor);
        if (bMatch)
        {
            if (bDelta)
            {


                if (KeyGetAttrib(tKey) != KeyGetAttrib(ATTRIBUTE_NONE))
                {
                    int32 nValueBefore = FastAttribGetValueInt(idFastAttrib, tKey);
                    FastAttribSetValueInt(idFastAttrib, tKey, 0);
                    UISkills::ResolvePowerTag(ptACD, idFastAttrib, -1, szTag, flResultAfter, NULL, strResult, snoPower, nLevel, pData, NULL, eFontColor);
                    FastAttribSetValueInt(idFastAttrib, tKey, nValueBefore);
                }

                flResult = flResultBefore - flResultAfter;
            }
            else
            {
                flResult = flResultBefore;
            }
        }
    }

    return bMatch;
}


//@ 227
static CRefString sGetSpellRuneAttributeDescriptions(
    const ActorCommonData & tACDOwner,
    const AttribGroupID idFastAttrib,
    SNO snoPower,
    const BOOL fAdvancedTooltip,
    const BOOL fCombinedTooltip,
    BOOL & fFoundCombinedTooltip)
{
    CRefString usTip;

    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    fFoundCombinedTooltip = FALSE;

    for (int32 i = 0; i < scgNumSpellRuneData; i++)
    {
        const SpellRuneData * pSpellRuneData = &ctSpellRuneData[i];

        FastAttribKey tKey = MakeAttribKey(pSpellRuneData->eAttrib, snoPower);
        if (FastAttribExists(idFastAttrib, tKey))
        {
            CRefString usAttributeFormatSpecifier;

            if (snoPower != -1)
            {
                CRefString sSuffix = CRefString().Format("%s#%s", pSpellRuneData->pszName, SNOToString(POWER_SNO, snoPower, FALSE).str());
                if (!fAdvancedTooltip)
                {
                    if (fCombinedTooltip &&
                        StringListFetch(snoAttributeStringList, CRefString().Format("Combined_Simple_%s", sSuffix.str()), usAttributeFormatSpecifier))
                    {
                        fFoundCombinedTooltip = TRUE;
                    }
                    else
                    {
                        CRefString usLabel = CRefString().Format("Simple_%s", sSuffix.str());
                        if (!StringListFetch(snoAttributeStringList, usLabel, usAttributeFormatSpecifier))
                        {
                            usTip.Format("Missing item attribute string label (%s)\n", usLabel.str());
                            return usTip;
                        }
                    }
                }
                else
                {
                    if (fCombinedTooltip &&
                        StringListFetch(snoAttributeStringList, CRefString().Format("Combined_%s", sSuffix.str()), usAttributeFormatSpecifier))
                    {
                        fFoundCombinedTooltip = TRUE;
                    }
                    else
                    {
                        if (!StringListFetch(snoAttributeStringList, sSuffix, usAttributeFormatSpecifier))
                        {
                            usTip.Format("Missing item attribute string label (%s)\n", sSuffix.str());
                            return usTip;
                        }
                    }
                }
            }
            else
            {
                CRefString usLabel = pSpellRuneData->pszName;
                if (!StringListFetch(snoAttributeStringList, usLabel, usAttributeFormatSpecifier))
                {
                    usTip.Format("Missing item attribute string label (%s)\n", usLabel.str());
                    return usTip;
                }
            }

            usTip += UIToolTips_ProcessTip(
                &tACDOwner,
                idFastAttrib,
                -1,
                snoPower,
                tACDOwner.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower)),
                usAttributeFormatSpecifier,
                TTVC_NORMAL_COLORING,
                sResolveSpellRuneTag,
                (void *)(int64)pSpellRuneData->eAttrib,
                NULL);
            return usTip;
        }
    }

    return usTip;
}


















































































//@ 396
int32 ItemGetSpecialAffixCount(
    const ActorCommonData & tACD,
    const PlayerClass eClass)
{
    int32 nCount = 0;
    for (int32 i = 0; i < cgNumTooltipAttribsTrailing; ++i)
    {
        CRefString usAttribString = "";
        const AttribTooltipInfo & tInfo = ctAttribTooltipInfoTrailing[i];
        usAttribString = GetAttributeDescription(tInfo, tACD, tACD.idFastAttrib, eClass, TRUE, (ItemDescriptionColorOverride)0, NULL);
        if (usAttribString.Length() != 0)
        {
            nCount++;
        }
    }
    return nCount;
}




























































//@ 474
int32 ItemCompareTooltipAttributes(
    const FastAttribKey & tKey1,
    const FastAttribKey & tKey2)
{
    int32 nKeyIndex1 = INT_MAX;
    for (int32 i = 0; i < cgNumTooltipAttribsPrimary && nKeyIndex1 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoPrimary[i].arKey[j] == tKey1)
            {
                nKeyIndex1 = i;
                break;
            }
        }
    }
    for (int32 i = 0; i < cgNumTooltipAttribsSecondary && nKeyIndex1 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoSecondary[i].arKey[j] == tKey1)
            {
                nKeyIndex1 = i;
                break;
            }
        }
    }

    for (int32 i = 0; i < cgNumTooltipAttribsLegendary && nKeyIndex1 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoLegendary[i].arKey[j] == tKey1)
            {
                nKeyIndex1 = i;
                break;
            }
        }
    }
    for (int32 i = 0; i < cgNumTooltipAttribsTrailing && nKeyIndex1 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoTrailing[i].arKey[j] == tKey1)
            {
                nKeyIndex1 = i;
                break;
            }
        }
    }


    int32 nKeyIndex2 = INT_MAX;
    for (int32 i = 0; i < cgNumTooltipAttribsPrimary && nKeyIndex2 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoPrimary[i].arKey[j] == tKey2)
            {
                nKeyIndex2 = i;
                break;
            }
        }
    }
    for (int32 i = 0; i < cgNumTooltipAttribsSecondary && nKeyIndex2 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoSecondary[i].arKey[j] == tKey2)
            {
                nKeyIndex2 = i;
                break;
            }
        }
    }

    for (int32 i = 0; i < cgNumTooltipAttribsLegendary && nKeyIndex2 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoLegendary[i].arKey[j] == tKey2)
            {
                nKeyIndex2 = i;
                break;
            }
        }
    }
    for (int32 i = 0; i < cgNumTooltipAttribsTrailing && nKeyIndex2 == INT_MAX; i++)
    {
        for (int32 j = 0; j < 3; j++)
        {
            if (ctAttribTooltipInfoTrailing[i].arKey[j] == tKey2)
            {
                nKeyIndex2 = i;
                break;
            }
        }
    }

    if (nKeyIndex1 < nKeyIndex2)
    {
        return -1;
    }
    return nKeyIndex2 < nKeyIndex1;
}

// NON_MATCHING: 680 vs 660 bytes. Same calls and control flow; the original computes the single-invalid return value (right invalid ? -1 : 1) with a cneg before the invalid-attribute tests and shares it between both exits, while ours sinks it after the tests, adding a cset and a cmp/cneg per unrolled iteration. Tried: separate ifs vs combined condition, else-if chain. f19: the original cneg carries row 631:18 (the || condition start), the location FoldTwoEntryPHINode gives a select it creates from a branch, so the original's return value there was a phi merged at the condition; ours keeps the clang ternary select in a block shared by all four unrolled iterations. Also tried swapped || operands, left-based ternary, if/return pair: no change or worse.
int32 ItemCompareAffixes(
    const void * lhs,
    const void * rhs)
{
    const GBID * ptLeft = (const GBID *)lhs;
    ScopedItemAffixLockR pdefLeft(*ptLeft);

    const GBID * ptRight = (const GBID *)rhs;
    ScopedItemAffixLockR pdefRight(*ptRight);

    if (!pdefLeft || !pdefRight)
    {


        return pdefLeft ? -1 : (pdefRight != NULL);
    }




















    if (pdefLeft->eAffixGroup != pdefRight->eAffixGroup)
    {
        return pdefLeft->eAffixGroup < pdefRight->eAffixGroup ? -1 : 1;
    }

    for (int32 i = 0; i < MAX_AFFIX_ATTRIBUTES; i++)
    {
        BOOL bLeftAttributeInvalid = pdefLeft->ptAffixAttributes[i].eAttribute == ATTRIBUTE_NONE;
        BOOL bRightAttributeInvalid = pdefRight->ptAffixAttributes[i].eAttribute == ATTRIBUTE_NONE;

        if (bLeftAttributeInvalid && bRightAttributeInvalid)
        {
            return 0;
        }
        else if (bLeftAttributeInvalid || bRightAttributeInvalid)
        {
            return bRightAttributeInvalid ? -1 : 1;
        }





        FastAttribKey tKeyLeft = MakeAttribKey(pdefLeft->ptAffixAttributes[i].eAttribute,
                                               pdefLeft->ptAffixAttributes[i].nParam);
        FastAttribKey tKeyRight = MakeAttribKey(pdefRight->ptAffixAttributes[i].eAttribute,
                                                pdefRight->ptAffixAttributes[i].nParam);
        int32 nTooltipCompare = ItemCompareTooltipAttributes(tKeyLeft, tKeyRight);
        if (nTooltipCompare != 0)
        {
            return nTooltipCompare;
        }
    }

    return 0;
}






































































//@ 724
CRefString ItemGetCostString(
    const ActorCommonData & tACDItem,
    const CostType eCostType,
    const ActorCommonData * ptACDContainer,
    BOOL bNumericOnly)
{
    CRefString usRetval;
    if (eCostType == -1)
    {
        return usRetval;
    }

    BOOL fConsiderStackSize = (eCostType != 1);







    CRefString usCost;
    if (ItemIsUnidentified(tACDItem))
    {
        usCost = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SHOP), "MysteryValue");
    }
    else
    {
        CurrencyType eCurrencyType = ptACDContainer ? ptACDContainer->GetCurrencyType() : 0;
        uint64 uCost = ItemGetCost(tACDItem, eCostType, eCurrencyType, ptACDContainer, fConsiderStackSize, FALSE);
        usCost = UIText_ValueToGameCurrency(uCost, eCurrencyType);
    }

    if (bNumericOnly)
        return usCost;
    switch (eCostType)
    {
    case 1:
    case 2:
        usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SHOP), "Cost");
        break;
    case 0: {
        ItemSellError eSellError = ItemCanBeSold(tACDItem, ptACDContainer);
        switch (eSellError)
        {
        case ISE_NONE:
        case ISE_UNIDENTIFIED:
            usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SHOP), "SellValue");
            break;
        default:
            usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SHOP), "SellValueCannotSell");
            break;
        }
        break;
    }
    }
    usRetval = StringListEntryReplace(usRetval, "{COST}", usCost, 999);
    return usRetval;
}































//@ 814
CRefString ItemGetCooldownTooltip(
    const ActorCommonData & tACDItem,
    const ActorCommonData & tACDPlayer)
{
    CRefString usRetval;

    uint32 dwCooldownTime = ItemGetRemainingCooldownTime(tACDPlayer, tACDItem);
    uint32 dwTimeLeft;
    if (dwCooldownTime != (uint32)-1)
    {
        dwTimeLeft = dwCooldownTime - CGameGetPredictedServerTime();
    }
    else
    {
        dwTimeLeft = ItemGetCooldownTimeToApply(tACDItem);
        if (dwTimeLeft == 0)
        {
            return "";
        }
    }

    if (dwTimeLeft != (uint32)-1)
    {

        CRefString usValue;
        CRefString sLabel;

        if (dwTimeLeft > 3600)
        {
            usValue.Format("%d", dwTimeLeft / 3600 + 1);
            sLabel = (dwCooldownTime != (uint32)-1) ? "CooldownRemainingMinutes" : "CooldownMinutes";
        }
        else
        {
            usValue.Format("%d", dwTimeLeft / 60);
            sLabel = (dwCooldownTime != (uint32)-1) ? "CooldownRemainingSeconds" : "CooldownSeconds";
        }

        CRefString usCooldown = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), sLabel.str());
        usRetval += StringListEntryReplace(usCooldown, "{TIME}", usValue, 999);
        usRetval += "\n";

        if (dwCooldownTime != (uint32)-1)
        {
            Set_Text_Color(usRetval, (FontSpecifier)1);
        }
        return usRetval;
    }
    return "";
}































extern "C" BOOL sub_71004f71e0(const ActorCommonData & tACDItem);
CRefString ItemGetItemTypeDescription(
    const ActorCommonData & tACDItem,
    const PlayerClass ePlayerClass)
{
    CRefString sItemType = ItemTypeGetDescription(tACDItem);

    if (ePlayerClass != -1
        && !ItemMeetsOwnerClassRestrictions(tACDItem.hGB.gbid, ePlayerClass, FALSE))
    {
        Set_Text_Color(sItemType, (FontSpecifier)1);
    }
    else
    {
        RGBAColor rgbaColor = ItemGetDisplayNameColor(tACDItem);
        if (sub_71004f71e0(tACDItem))
            rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
        Set_Text_Color(sItemType, rgbaColor);
    }

    return sItemType;
}



















//@ 937
CRefString PlayerGetSpellRuneToolTip(
    const ActorCommonData & tACDPlayer,
    const SNO snoPower,
    const RuneType eRune,
    const BOOL fAdvancedTooltip,
    const BOOL fCombinedTooltip,
    BOOL & fFoundCombinedTooltip)
{
    CRefString sRuneTip;


    AttribGroupID idTempAttrib = FastAttribLightCreate(0);

    FastAttribLightCopy(tACDPlayer.idFastAttrib, idTempAttrib);
    FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower), eRune == RUNE_TYPE_A);
    FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower), eRune == RUNE_TYPE_B);
    FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower), eRune == RUNE_TYPE_C);
    FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower), eRune == RUNE_TYPE_D);
    FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower), eRune == RUNE_TYPE_E);



    SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
    if (snoSynergyPower != -1)
    {
        FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, snoSynergyPower), eRune == RUNE_TYPE_A);
        FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, snoSynergyPower), eRune == RUNE_TYPE_B);
        FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, snoSynergyPower), eRune == RUNE_TYPE_C);
        FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, snoSynergyPower), eRune == RUNE_TYPE_D);
        FastAttribSetValueInt(idTempAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, snoSynergyPower), eRune == RUNE_TYPE_E);
    }

    CRefString usAttribString = sGetSpellRuneAttributeDescriptions(tACDPlayer, idTempAttrib, snoPower, fAdvancedTooltip, fCombinedTooltip, fFoundCombinedTooltip);
    if (usAttribString.Length() != 0)
    {
        usAttribString += "\n";
    }
    sRuneTip += usAttribString;


    FastAttribGroupDelete(idTempAttrib);

    return sRuneTip;
}




















































































































































































































































































//@ 1258
CRefString ItemGetTradableToText(
    const ActorCommonData & tACDItem)
{


    CRefString usRetval;
    uint32 uTimeLeft = ItemGetTradeTimeLeft(tACDItem);
    uint32 uHours = uTimeLeft / (60 * 60 * 60);
    uint32 uMinutes = (uTimeLeft % (60 * 60 * 60)) / (60 * 60);
    uint32 uSeconds = (uTimeLeft % (60 * 60)) / 60;

    CRefString usTimeLeft = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ItemTradableTime");
    usTimeLeft = StringListEntryReplace(usTimeLeft, "{HOURS}", CRefString().Format("%d", uHours), 999);
    usTimeLeft = StringListEntryReplace(usTimeLeft, "{MINUTES}", CRefString().Format("%d", uMinutes), 999);
    usTimeLeft = StringListEntryReplace(usTimeLeft, "{SECONDS}", CRefString().Format("%d", uSeconds), 999);
    Set_Text_Color(usTimeLeft, (FontSpecifier)0x11);
    usRetval += usTimeLeft + CRefString("\n");
    return usRetval;
}
































//@ 1310
BOOL ItemIsBeingManipulated(
    const ActorCommonData & tACDItem)
{

    uint32 dwTimeout = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_MANIPULATION_TIMEOUT);
    return dwTimeout && CGameGetClientTicks() < dwTimeout;
}

//@ 1319
void ItemSetIsBeingManipulated(
    ActorCommonData & tACDItem,
    const BOOL fVal,
    const uint32 dwTimeout)
{
    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_MANIPULATION_TIMEOUT, fVal ? CGameGetClientTicks() + dwTimeout : 0);
}
