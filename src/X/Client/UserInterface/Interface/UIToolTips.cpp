// X/Client/UserInterface/Interface/UIToolTips.cpp
//
// Tooltip callbacks for the hero and follower details panels. Each `UIToolTips_*` entry point
// resolves the local hero (or the current follower), formats the stat it describes and hands the
// localized text to the shared tooltip manager.

#include "UIToolTips.h"

#include "UIHirelingCommon.h"
#include "UIToolTipManager.h"
#include "../UIID.h"
#include "../Controls/UIStackPanelControl.h"
#include "../../CPlayer.h"
#include "../../RActors/RActors.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorCommonDataSwig.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HirelingSwig.h"
#include "../../../Common/Powers/PowerAPISwig.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
ACDID LocalPlayerACDGet();  // X/Client/CPlayer.cpp
CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel);  // X/Client/CPlayer.cpp
float HirelingGetDiminishedAttribute(ACDID idACDPlayer, Attrib eAttrib);  // X/Common/Hireling.cpp
float ExperienceGetBonusFromGear(const ActorCommonData & tACDPlayer);  // X/Common/ExperienceCommon.cpp
float PowerComputeResistanceDamageMitigation(float flResistanceTotal, int32 nAttackerLevel);  // X/Common/Powers/PowersCommon.cpp
uint64 ExperienceGetCurrentLevelXP(const ActorCommonData & tACD, const BOOL bUseAlt);  // X/Common/ExperienceCommon.cpp
BOOL ACDIsDualWielding(const ActorCommonData & tACD, BOOL bUseVisualInventory);  // X/Common/ActorInventory.cpp
namespace UIHireling {
ACDID GetCurrentHireling();  // X/Client/UserInterface/Interface/UIHireling.cpp
}
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace UISkills {
BOOL GetName(SNO snoPower, CRefString & sSkillName);  // X/Common/UserInterface/UISkillsCommon.cpp
}
namespace UIInventory {
CRefString GetDPSString(float flDPS);  // X/Client/UserInterface/Interface/UIInventoryCommon.cpp
}

void UIToolTips_Nothing(const UIID &)
{
}

CRefString sTooltipExperienceForACD(ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    int32 nAltLevel = tACD.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
    BOOL bUseAltLevel = tACD.ShouldUseAltLevels();

    CRefString usLevel = CPlayerGetLevelString(nLevel, nAltLevel);

    uint64 nXPCurrent;
    uint64 nXPTotalForLevel;

    CRefString usExpTip;
    if (bUseAltLevel)
    {
        int32 nAltLevelScoped = tACD.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
        CRefString usAltLevel = UnicodeText::FormatNumber(nAltLevelScoped, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        if (nAltLevelScoped < 20000)
        {
            nXPCurrent = ExperienceGetCurrentLevelXP(tACD, TRUE);
            uint64 nXPRemainingToLevel = tACD.GetAltExperienceNext();
            nXPTotalForLevel = nXPRemainingToLevel + nXPCurrent;
        }
        else
        {
            nXPCurrent = 0;
            nXPTotalForLevel = 0;
        }

        CRefString usCurrent = UnicodeText::FormatNumber(nXPCurrent, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString usXPToLevel = UnicodeText::FormatNumber(nXPTotalForLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);

        usExpTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "TipAltExperience"), usCurrent.str(), usXPToLevel.str(), usLevel.str(), NULL);
    }
    else
    {
        int32 nLevelCap = tACD.AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
        if (nLevel < nLevelCap)
        {
            nXPCurrent = ExperienceGetCurrentLevelXP(tACD, FALSE);
            uint64 nXPRemainingToLevel = tACD.GetExperienceNext();
            nXPTotalForLevel = nXPRemainingToLevel + nXPCurrent;
        }
        else
        {
            nXPCurrent = 0;
            nXPTotalForLevel = 0;
        }

        CRefString usCurrent = UnicodeText::FormatNumber(nXPCurrent, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString usXPToLevel = UnicodeText::FormatNumber(nXPTotalForLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);

        usExpTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "TipExperience"), usCurrent.str(), usXPToLevel.str(), usLevel.str(), NULL);
    }

    uint64 nRestXP = tACD.GetRestExperience();
    if (nRestXP)
    {
        float flRestXPBonusPct = tACD.AttributesGetFloat(ATTRIBUTE_REST_EXPERIENCE_BONUS_PCT) * 100.0f;
        CRefString usRestXPBonusPct = UnicodeText::FormatNumber(flRestXPBonusPct, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usRestXP = UnicodeText::FormatNumber(nRestXP, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString usRestXPTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "TipBonusExperience"), usRestXPBonusPct.str(), usRestXP.str(), NULL);
        usExpTip += "\n";
        usExpTip += usRestXPTip;
    }

    return usExpTip;
}

inline CRefString sGetTipLabelPrefix(const HirelingClass eHirelingClass)
{
    CRefString sLabel;
    switch (eHirelingClass)
    {
    case HC_TEMPLAR:
        return "TipTemplar";
    case HC_SCOUNDREL:
        return "TipScoundrel";
    case HC_ENCHANTRESS:
        return "TipEnchantress";
    default:
        break;
    }
    return "Tip";
}

void sStrength(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    float flVal = tACD.AttributesGetFloat(ATTRIBUTE_STRENGTH_TOTAL);
    float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usArmorBonus = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Strength";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabel.str()), usVal.str(), usValBonus.str(), usArmorBonus.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroStrength(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sStrength(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_HirelingStrength(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sStrength(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void sStrengthBonus(const UIID & id, const ActorCommonData & tACD)
{
    float flVal = 0.0f;
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipStrengthBonus"), usVal.str(), usVal.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroStrengthBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sStrengthBonus(id, *ptACD);
    }
}

void UIToolTips_HirelingStrengthBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sStrengthBonus(id, *ptACD);
    }
}

void sDexterity(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    float flVal = tACD.AttributesGetFloat(ATTRIBUTE_DEXTERITY_TOTAL);
    float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Dexterity";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabel.str()), usVal.str(), usValBonus.str(), usVal.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroDexterity(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDexterity(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_HirelingDexterity(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDexterity(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void sDexterityBonus(const UIID & id, const ActorCommonData & tACD)
{
    float flVal = 0.0f;
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    float flCritDmg = PowerComputeCritDamageScalar(&tACD) - 1.0f;
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usCritDmg = UnicodeText::FormatNumber(flCritDmg, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipDexterityBonus"), usVal.str(), usVal.str(), usCritDmg.str(), usLevel.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroDexterityBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDexterityBonus(id, *ptACD);
    }
}

void UIToolTips_HirelingDexterityBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDexterityBonus(id, *ptACD);
    }
}

void sIntelligence(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    float flVal = tACD.AttributesGetFloat(ATTRIBUTE_INTELLIGENCE_TOTAL);
    float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
    float flResistBonus = tACD.AttributesGetFloat(ATTRIBUTE_RESIST_FROM_INTELLIGENCE);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usResistanceBonus = UnicodeText::FormatNumber(flResistBonus, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Intelligence";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabel.str()), usVal.str(), usValBonus.str(), usResistanceBonus.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroIntelligence(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sIntelligence(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_HirelingIntelligence(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sIntelligence(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void sIntelligenceBonus(const UIID & id, const ActorCommonData & tACD)
{
    float flVal = 0.0f;
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipIntelligenceBonus"), usVal.str(), usLevel.str(), usVal.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroIntelligenceBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sIntelligenceBonus(id, *ptACD);
    }
}

void UIToolTips_HirelingIntelligenceBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sIntelligenceBonus(id, *ptACD);
    }
}

void sVitality(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    float flVal = tACD.AttributesGetFloat(ATTRIBUTE_VITALITY_TOTAL);
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_FACTOR_VITALITY);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Vitality";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabel.str()), usVal.str(), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroVitality(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sVitality(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_HirelingVitality(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sVitality(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void sVitalityBonus(const UIID & id, const ActorCommonData & tACD)
{
    int32 nValCur = Math::FloatRoundToInt(Math::Max(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR), 0.0f));
    int32 nValMax = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_FACTOR_VITALITY);
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    CRefString usValCur = UnicodeText::FormatNumber(nValCur, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usValMax = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipVitalityBonus"), usValCur.str(), usValMax.str(), usStat.str(), usLevel.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroVitalityBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sVitalityBonus(id, *ptACD);
    }
}

void UIToolTips_HirelingVitalityBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sVitalityBonus(id, *ptACD);
    }
}

static void sArmor(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    float flVal = (float)Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_ARMOR_TOTAL));
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    float flStat = PowerComputeArmorDamageMitigation(tACD, nCurrentLevel);
    CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Armor";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabel.str()), usVal.str(), usLevel.str(), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_HeroSurvivability(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flVal = ptACD->GetEHPValue();
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipSurvivability"), usVal.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HeroDPS(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        BOOL bWasDualWielding = ACDIsDualWielding(*ptACD, TRUE);
        float flVal = ptACD->GetDPSValue(bWasDualWielding);
        CRefString usVal = UIInventory::GetDPSString(flVal);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipDPS"), usVal.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HeroHealing(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flBaseHealing;
        BOOL bWasDualWielding = ACDIsDualWielding(*ptACD, TRUE);
        flBaseHealing = 0.0f;
        float flRecovery = ptACD->GetHealingValue(bWasDualWielding, &flBaseHealing);
        CRefString usRecovery = UnicodeText::FormatNumber(flRecovery, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usBaseHealing = UnicodeText::FormatNumber(flBaseHealing, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipHealing"), usRecovery.str(), usBaseHealing.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingArmor(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sArmor(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void UIToolTips_HirelingSurvivability(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        float flVal = ptACD->GetEHPValue();
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipSurvivability"), usVal.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDPS(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        float flDamage;
        float flAttackSpeed;
        PowerActorGetWeaponDamageAndAttackSpeed(*ptACD, flDamage, flAttackSpeed, TRUE, -1, TRUE);
        float flBaseDamageScalar = UIHireling::GetBaseDamageScalarForClass(*ptACD);
        float flDPS = flDamage * flAttackSpeed * flBaseDamageScalar;
        CRefString usVal = UIInventory::GetDPSString(flDPS);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipDPS"), usVal.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void sDetailsDamageBonus(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = PowerComputeDamageScalarFromPrimaryAttribute(tACD.idFastAttrib);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTipName;
    PrimaryDamageAttribute ePrimaryDamageAttribute = tACD.AttributesGetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE);
    switch (ePrimaryDamageAttribute)
    {
    case PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH:
        usTipName = "TipDamageBonusStrength";
        break;
    case PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY:
        usTipName = "TipDamageBonusDexterity";
        break;
    case PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE:
        usTipName = "TipDamageBonusIntelligence";
        break;
    }
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), usTipName.str()), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsDamageBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsDamageBonus(id, *ptACD);
    }
}

void UIToolTips_DetailsDamageBonusSkills(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageBonusSkills"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsBonusDamageElites(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusDamageElites"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsDamageBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsDamageBonus(id, *ptACD);
    }
}

void sDetailsAttackSpeedPercent(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_SUBTOTAL) - 1.0f;
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeedPercent"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsAttackSpeedPercent(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsAttackSpeedPercent(id, *ptACD);
    }
}

void sDetailsAttackSpeed(const UIID & id, const ActorCommonData & tACD, BOOL bOffHand)
{
    if (!ACDIsDualWielding(tACD, TRUE))
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeed"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
    else
    {
        int32 nCurrentHand = FastAttribGetValueInt(tACD.idFastAttrib, ATTRIBUTE_DUALWIELD_HAND);
        if (!bOffHand)
        {
            FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_DUALWIELD_HAND, 0);
            float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeed"), usStat.str(), NULL);
            gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
        }
        else
        {
            FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_DUALWIELD_HAND, 1);
            float flStatOffHand = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
            CRefString usStatOffHand = UnicodeText::FormatNumber(flStatOffHand, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeed"), usStatOffHand.str(), NULL);
            gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
        }
        FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_DUALWIELD_HAND, nCurrentHand);
    }
}

void UIToolTips_DetailsAttackSpeed(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsAttackSpeed(id, *ptACD, FALSE);
    }
}

void UIToolTips_DetailsAttackSpeedOffHand(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsAttackSpeed(id, *ptACD, TRUE);
    }
}

void UIToolTips_HirelingDetailsAttackSpeed(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsAttackSpeed(id, *ptACD, FALSE);
    }
}

void sDetailsCritChance(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = PowerComputeCritChance(&tACD, FALSE);
    float flDmg = PowerComputeCritDamageScalar(&tACD) - 1.0f;
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usDmg = UnicodeText::FormatNumber(flDmg, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCritChance"), usStat.str(), usDmg.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsCritChance(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsCritChance(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsCritChance(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsCritChance(id, *ptACD);
    }
}

void sDetailsCritDamageBonus(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = PowerComputeCritDamageScalar(&tACD) - 1.0f;
    CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCritDamageBonus"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsCritDamageBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsCritDamageBonus(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsCritDamageBonus(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsCritDamageBonus(id, *ptACD);
    }
}

void sDetailsSplashDamage(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    ScopedGlobalDefinitionLockR pdefGlobal;
    float flSplashChance = pdefGlobal->flSplashDamageEffectProcChance;
    CRefString usChance = UnicodeText::FormatNumber(flSplashChance, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
    float flSplashRadius = pdefGlobal->flSplashDamageEffectRadius;
    CRefString usRadius = UnicodeText::FormatNumber(flSplashRadius, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSplashDamage"), usStat.str(), usChance.str(), usStat.str(), usRadius.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsSplashDamage(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsSplashDamage(id, *ptACD);
    }
}

void sDetailsCrushingBlow(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_CRUSHING_BLOW_PROC_CHANCE);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    ScopedGlobalDefinitionLockR pdefGlobal;
    float flCrushingBlowDamage = pdefGlobal->flCrushingBlowDamagePercentNormal;
    CRefString usDamage = UnicodeText::FormatNumber(flCrushingBlowDamage, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCrushingBlow"), usStat.str(), usDamage.str());
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsCrushingBlow(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsCrushingBlow(id, *ptACD);
    }
}

void sDetailsMovementSpeed(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_MOVEMENT_SCALAR_TOTAL) - 1.0f;
    CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    float flStatPlus100 = flStat + 1.0f;
    CRefString usStatPlus100 = UnicodeText::FormatNumber(flStatPlus100, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMovementSpeed"), usStat.str(), usStatPlus100.str());
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsMovementSpeed(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsMovementSpeed(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsMovementSpeed(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsMovementSpeed(id, *ptACD);
    }
}

void UIToolTips_DetailsGoldFind(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_GOLD_FIND_TOTAL) + HirelingGetDiminishedAttribute(idACD, ATTRIBUTE_GOLD_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipGoldFind"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsGoldFind(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRatio = pdefGlobal->flHirelingDiminishedBonusRatio;
        float flStat = flRatio * ptACD->AttributesGetFloat(ATTRIBUTE_GOLD_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWER_DETAILS), "TipGoldFind"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsMagicFind(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_TOTAL) + HirelingGetDiminishedAttribute(idACD, ATTRIBUTE_MAGIC_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMagicFind"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsMagicFind(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRatio = pdefGlobal->flHirelingDiminishedBonusRatio;
        float flStat = flRatio * ptACD->AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_TOTAL);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWER_DETAILS), "TipMagicFind"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsBonusXP(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ExperienceGetBonusFromGear(*ptACD) + HirelingGetDiminishedAttribute(idACD, ATTRIBUTE_EXPERIENCE_BONUS_PCT_TOTAL);

        float flStat2 = ptACD->AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS_PCT_SUPER_SCALAR_TOTAL);

        float flStatTotal = flStat * flStat2 - 1.0f;

        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
        const TieredLootRunLevel & tTieredLootRunLevel = *pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels;
        float flStat3 = 1.0f - tTieredLootRunLevel.tLootRunValues.flXPReductionScalar;

        CRefString usTotal = UnicodeText::FormatNumber(flStatTotal, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
        CRefString usStat = UnicodeText::FormatNumber(flStat - 1.0f, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
        CRefString usStat2 = UnicodeText::FormatNumber(flStat2, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usStat3 = UnicodeText::FormatNumber(flStat3, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusXP"), usTotal.str(), usStat.str(), usStat2.str(), usStat3.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsBonusXPPerKill(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS) + HirelingGetDiminishedAttribute(idACD, ATTRIBUTE_EXPERIENCE_BONUS) + ptACD->AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS_NO_PENALTY);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusXPPerKill"), usStat.str(), usStat.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsCCReduction(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_REDUCTION);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCCReduction"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsMissileDefenseBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMissileDefenseBonus"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsMeleeDefenseBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMeleeDefenseBonus"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UI_ToolTip_DetailsThornsFixed(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = 0.0f;
        for (int eDamageType = 0; eDamageType < NUM_DAMAGE_TYPES; eDamageType++)
        {
            flStat += ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_THORNS_FIXED, eDamageType));
        }
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipThornsFixed"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void sDetailsBlockAmount(const UIID & id, const ActorCommonData & tACD)
{
    float flStatMin = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MIN);
    float flStatMax = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MAX);
    CRefString usStatMin = UnicodeText::FormatNumber(flStatMin, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usStatMax = UnicodeText::FormatNumber(flStatMax, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBlockAmount"), usStatMin.str(), usStatMax.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsArmor(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sArmor(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_DetailsBlockAmount(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsBlockAmount(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsBlockAmount(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsBlockAmount(id, *ptACD);
    }
}

void sDetailsBlockChance(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_CHANCE_CAPPED_TOTAL);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
    float flBlockAmountMin = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MIN);
    float flBlockAmountMax = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MAX);
    CRefString sBlockAmountMin = UnicodeText::FormatNumber(flBlockAmountMin, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString sBlockAmountMax = UnicodeText::FormatNumber(flBlockAmountMax, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBlockChance"), usStat.str(), sBlockAmountMin.str(), sBlockAmountMax.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsBlockChance(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsBlockChance(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsBlockChance(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsBlockChance(id, *ptACD);
    }
}

void sDetailsDodgeChance(const UIID & id, const ActorCommonData & tACD)
{
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    float flStat = Math::Max(PowerComputeDodgeChance(tACD, nCurrentLevel, PDM_MELEE, FALSE), 0.0f);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDodgeChance"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsDodgeChance(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsDodgeChance(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsDodgeChance(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsDodgeChance(id, *ptACD);
    }
}

void sDetailsDamageReduction(const UIID & id, const ActorCommonData & tACD)
{
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    float flStat = PowerComputeArmorDamageMitigation(tACD, nCurrentLevel);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageReduction"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsDamageReduction(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsDamageReduction(id, *ptACD);
    }
}

void UIToolTips_DetailsEliteDamageReduction(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipEliteDamageReduction"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsDamageReduction(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsDamageReduction(id, *ptACD);
    }
}

void sDetailsLife(const UIID & id, const ActorCommonData & tACD, const HirelingClass eHirelingClass)
{
    int32 nValCur = Math::FloatRoundToInt(Math::Max(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR), 0.0f));
    int32 nValMax = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
    CRefString usValCur = UnicodeText::FormatNumber(nValCur, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString usValMax = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);

    CRefString sLabel = sGetTipLabelPrefix(eHirelingClass) + "Life";
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel.str()), usValCur.str(), usValMax.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsLife(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsLife(id, *ptACD, HC_NONE);
    }
}

void UIToolTips_DetailsLifeFromAffix(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS) + ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS_ITEM);
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeFromAffix"), usStat.str(), usStat.str());
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsLife(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    const ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsLife(id, *ptACD, ACDGetHirelingClass(*ptACD));
    }
}

void sDetailsLifeRegen(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_SUBTOTAL) + tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS_FROM_SKILLS);
    float flHealingModifier = 1.0f + tACD.AttributesGetFloat(ATTRIBUTE_BONUS_HEALING_RECEIVED_PERCENT) - tACD.AttributesGetFloat(ATTRIBUTE_REDUCED_HEALING_RECEIVED_PERCENT);
    CRefString usStat = UnicodeText::FormatNumber(flStat * flHealingModifier, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeRegen"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsLifeRegen(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsLifeRegen(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsLifeRegen(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsLifeRegen(id, *ptACD);
    }
}

void sDetailsLifeSteal(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeSteal"), usStat.str(), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsLifeSteal(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsLifeSteal(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsLifeSteal(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsLifeSteal(id, *ptACD);
    }
}

void sDetailsLifePerKill(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_KILL);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerKill"), usStat.str(), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsLifePerKill(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsLifePerKill(id, *ptACD);
    }
}

void UIToolTips_HirelingDetailsLifePerKill(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsLifePerKill(id, *ptACD);
    }
}

void sDetailsLifePerHit(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerHit"), usStat.str(), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsLifePerHit(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsLifePerHit(id, *ptACD);
    }
}

void UIToolTips_DetailsLifePerResourceSpent(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    CRefString usTip;
    if (ptACD)
    {
        ResourceType eResourceType = ptACD->GetPrimaryResourceType();
        switch (eResourceType)
        {
        case RESOURCE_TYPE_FURY:
        {
            float flHealPercent = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_FURY);
            CRefString usStat = UnicodeText::FormatNumber(flHealPercent, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerFurySpent"), usStat.str(), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_SPIRIT:
        {
            float flHealPercent = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_SPIRIT);
            CRefString usStat = UnicodeText::FormatNumber(flHealPercent, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerSpiritSpent"), usStat.str(), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_FAITH:
        {
            float flHealPercent = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_FAITH);
            CRefString usStat = UnicodeText::FormatNumber(flHealPercent, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerFaithSpent"), usStat.str(), usStat.str(), NULL);
            break;
        }
        }
    }
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsHealthGlobeBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipHealthGlobeBonus"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsGlobeRadiusBonus(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_GOLD_PICKUP_RADIUS);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipGlobeRadiusBonus"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_HirelingDetailsLifePerHit(const UIID & id)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sDetailsLifePerHit(id, *ptACD);
    }
}

void UIToolTips_Experience(const UIID & id)
{
    RActorID idRActor = LocalPlayerRActorGet();
    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {
        gtTooltipMgr.SetTextToolTip(id, sTooltipExperienceForACD(ptRActor->idACD), 1, FALSE, TRUE);
    }
}

void sDetailsAPOC(const UIID & id, const ActorCommonData & tACD)
{
    float flStat = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_CRIT, RESOURCE_TYPE_ARCANUM);
    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipArcanePowerOnCrit"), usStat.str(), NULL);
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

void UIToolTips_DetailsAPOC(const UIID & id)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sDetailsAPOC(id, *ptACD);
    }
}

void UIToolTips_DetailsResourceRow1(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        ResourceType eResourceType = ptACD->GetPrimaryResourceType();
        int32 nValMax = Math::FloatFloorToInt(ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType));
        CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString sLabel;
        switch (eResourceType)
        {
        case RESOURCE_TYPE_ARCANUM:
            sLabel = "TipMaxArcanum";
            break;
        case RESOURCE_TYPE_FURY:
            sLabel = "TipMaxFury";
            break;
        case RESOURCE_TYPE_SPIRIT:
            sLabel = "TipMaxSpirit";
            break;
        case RESOURCE_TYPE_HATRED:
            sLabel = "TipMaxHatred";
            break;
        case RESOURCE_TYPE_FAITH:
            sLabel = "TipMaxFaith";
            break;
        case RESOURCE_TYPE_ESSENCE:
            sLabel = "TipMaxEssence";
            break;
        default:
            sLabel = "TipMaxMana";
            break;
        }
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel.str()), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsResourceRow2(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        CRefString usTip;
        ResourceType eResourceType = ptACD->GetPrimaryResourceType();
        switch (eResourceType)
        {
        case RESOURCE_TYPE_FURY:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_FURY);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_FURY);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipFuryGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_MANA:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_MANA);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_MANA);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipManaGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_ARCANUM:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_ARCANUM);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_ARCANUM);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 1, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipArcanumGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_HATRED:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_HATRED);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_HATRED);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipHatredGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_SPIRIT:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_SPIRIT);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_SPIRIT);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSpiritGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_FAITH:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_FAITH);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_FAITH);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipWrathGain"), usStat.str(), NULL);
            break;
        }
        case RESOURCE_TYPE_ESSENCE:
        {
            float flRegenPerSecond = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_ESSENCE);
            float flRegenGainPct = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_ESSENCE);
            float flStat = flRegenPerSecond * (flRegenGainPct + 1.0f);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipEssenceGain"), usStat.str(), NULL);
            break;
        }
        }
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsResourceRow3(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        ResourceType eResourceType = ptACD->GetPrimaryResourceType();
        float flStat = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_TOTAL, eResourceType);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
        CRefString sLabel = CRefString().Format("Tip%sCostReduction", SymbolToName(eResourceType, gResourceTypes));
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel.str()), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsResourceRow4(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        CRefString usTip;
        ResourceType eSecondaryResourceType = ptACD->GetSecondaryResourceType();
        if (eSecondaryResourceType == RESOURCE_TYPE_DISCIPLINE)
        {
            int32 nValMax = Math::FloatFloorToInt(ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, RESOURCE_TYPE_DISCIPLINE));
            CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMaxDiscipline"), usStat.str(), NULL);
        }
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
        ResourceType ePrimaryResourceType = ptACD->GetPrimaryResourceType();
        if (ePrimaryResourceType == RESOURCE_TYPE_ARCANUM)
        {
            sDetailsAPOC(id, *ptACD);
        }
    }
}

void UIToolTips_DetailsResourceRow5(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        CRefString usTip;
        ResourceType eSecondaryResourceType = ptACD->GetSecondaryResourceType();
        if (eSecondaryResourceType == RESOURCE_TYPE_DISCIPLINE)
        {
            float flStat = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_DISCIPLINE);
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
            usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDisciplineGain"), usStat.str(), NULL);
        }
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsResourceRow6(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        ResourceType eResourceType = ptACD->GetSecondaryResourceType();
        float flStat = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_TOTAL, eResourceType);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
        CRefString sLabel = CRefString().Format("Tip%sCostReduction", SymbolToName(eResourceType, gResourceTypes));
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel.str()), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsCooldownReduction(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        float flStat = ptACD->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_PERCENT_ALL_CAPPED);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCooldownReduction"), usStat.str(), NULL);
        gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
    }
}

void UIToolTips_DetailsDamageTypeBonus(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        int32 nDamageType = (int32)(intptr_t)UI_GetStackPanelFromID(id).GetUserData();
        if (nDamageType >= 0 && nDamageType < NUM_DAMAGE_TYPES)
        {
            float flStat = ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, nDamageType));
            CRefString sDamageType;
            CRefString sLabel = CRefString().Format("Damage_%s", SymbolToName(nDamageType, gDamageTypes));
            sDamageType = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), sLabel.str());
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
            BOOL bWasDualWielding = ACDIsDualWielding(*ptACD, TRUE);
            float flVal = ptACD->GetDPSValue(bWasDualWielding);
            CRefString usDPS = UIInventory::GetDPSString((flStat + 1.0f) * flVal);
            CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageTypeBonus"), sDamageType.str(), usStat.str(), sDamageType.str(), usDPS.str());
            gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
        }
    }
}

void UIToolTips_DetailSkillBonus(const UIID & id)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD)
    {
        SNO snoPower = (SNO)(intptr_t)UI_GetStackPanelFromID(id).GetUserData();
        CRefString strPowerName;
        if (snoPower != -1 && UISkills::GetName(snoPower, strPowerName))
        {
            float flStat = ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS, snoPower));
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
            CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSkillBonus"), strPowerName.str(), usStat.str(), NULL);
            gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
        }
    }
}

void UIToolTips_Life(const UIID & id)
{
}

static void sToolTipResistanceShared(const UIID & id, const ActorCommonData & tACD, const DamageType eDamageType, const LPCSTR szLabel)
{
    float flResistanceTotal = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, eDamageType));
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    CRefString usResistanceTotal = UnicodeText::FormatNumber(flResistanceTotal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    float flVal = PowerComputeResistanceDamageMitigation(flResistanceTotal, nCurrentLevel);
    CRefString usStat = UnicodeText::FormatNumber(flVal, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
    CRefString usTip;
    float flBonusReduction = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, eDamageType));
    if (flBonusReduction > 0.000001f)
    {
        CRefString sLabelBonus = szLabel;
        sLabelBonus += "Bonus";
        CRefString usBonusPercent = UnicodeText::FormatNumber(flBonusReduction, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabelBonus.str()), usResistanceTotal.str(), usLevel.str(), usStat.str(), usBonusPercent.str(), NULL);
    }
    else
    {
        usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), szLabel), usResistanceTotal.str(), usLevel.str(), usStat.str(), NULL);
    }
    gtTooltipMgr.SetTextToolTip(id, usTip, 1, FALSE, TRUE);
}

static void sToolTipResistancePlayer(const UIID & id, const DamageType eDamageType, const LPCSTR szLabel)
{
    ACDID idACD = LocalPlayerACDGet();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        sToolTipResistanceShared(id, *ptACD, eDamageType, szLabel);
    }
}

void UIToolTips_Resistance_Physical(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_PHYSICAL, "TipPhysicalResist");
}

void UIToolTips_Resistance_Cold(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_COLD, "TipColdResist");
}

void UIToolTips_Resistance_Fire(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_FIRE, "TipFireResist");
}

void UIToolTips_Resistance_Poison(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_POISON, "TipPoisonResist");
}

void UIToolTips_Resistance_Lightning(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_LIGHTNING, "TipLightningResist");
}

void UIToolTips_Resistance_Arcane(const UIID & id)
{
    sToolTipResistancePlayer(id, DAMAGE_TYPE_ARCANE, "TipArcaneResist");
}

static void sToolTipResistanceHireling(const UIID & id, const DamageType eDamageType, const LPCSTR szLabel)
{
    ACDID idACDHireling = UIHireling::GetCurrentHireling();
    const ActorCommonData * ptACD = ACDTryToGet(idACDHireling);
    if (ptACD)
    {
        sToolTipResistanceShared(id, *ptACD, eDamageType, szLabel);
    }
}

void UIToolTips_HirelingResistance_Physical(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_PHYSICAL, "TipPhysicalResist");
}

void UIToolTips_HirelingResistance_Cold(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_COLD, "TipColdResist");
}

void UIToolTips_HirelingResistance_Fire(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_FIRE, "TipFireResist");
}

void UIToolTips_HirelingResistance_Poison(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_POISON, "TipPoisonResist");
}

void UIToolTips_HirelingResistance_Lightning(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_LIGHTNING, "TipLightningResist");
}

void UIToolTips_HirelingResistance_Arcane(const UIID & id)
{
    sToolTipResistanceHireling(id, DAMAGE_TYPE_ARCANE, "TipArcaneResist");
}
