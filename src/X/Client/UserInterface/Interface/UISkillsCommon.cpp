// Original file: X/Client/UserInterface/Interface/UISkillsCommon.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/Powers/PowersStruct.h"
#include "../../../Common/TagMaps/PowerTags.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../Definitions/PreferencesDefinition.h"
// Declared here because their headers are not adopted yet (signatures from the DWARF).
void CACDAttributeSetValue(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tValue, const BOOL fBypassRActorChanges);  // CActorCommonData.cpp:58
const TraitEntry * SkillKitGetMatchingTraitEntry(const SNO snoSkillKit, const SNO snoPower); const ActiveSkillEntry * SkillKitGetMatchingActiveSkillEntry(const SNO snoSkillKit, const SNO snoPower);  // SkillKitDefinition.cpp:214, 193
CRefString PowerTrace(SNO snoPower); /* PowersCommon.cpp */ CRefString StringListEntryReplace(LPCSTR szString, LPCSTR szToken, LPCSTR szReplacement, BOOL bReplaceAll);  // StringListDefinition.cpp
namespace UISkills {
BOOL ResolvePowerTag(const ActorCommonData * ptACD, AttribGroupID idFastAttrib, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString &, SNO snoPower, int32 nLevel, void *, void *, FontSpecifier &);  // X/Common/UserInterface/UISkillsCommon.cpp:32
CRefString GetSkillFlavorText(ActorCommonData & tACDOwner, const SNO snoPower, const int32 nLevel, const BOOL bLongerText);
CRefString GetSkillDescriptionText(ActorCommonData & tACDOwner, const SNO snoPower, const int32 nLevel, const BOOL bShowStats, const BOOL bLongerText); SkillCategory GetSkillCategoryForSlot(int32 nSlot, const ACDID idPlayer); SkillCategory GetSkillCategoryForSlot(int32 nSlot, const PlayerClass ePlayerClass); int32 SkillCategoryGetSlot(SkillCategory eCategory); CRefString GetSkillCategoryLabel(const SkillCategory eCategory); }



static CRefString sGetSkillTooltipLabel(CRefString & sSkillLabel, BOOL bForceComplexTooltip)
{


    CRefString sSkillTooltipLabel;
    if (!Prefs::GetGameplayPrefs().ShowAdvancedTooltips() && !bForceComplexTooltip)
    {
        sSkillTooltipLabel = CRefString().Format("%s_simple_desc", sSkillLabel.str());
    }
    else
    {
        sSkillTooltipLabel = CRefString().Format("%s_desc", sSkillLabel.str());
    }
    return sSkillTooltipLabel;
}


























static int32 sGetLevelReqForSkill(ActorCommonData & tACDOwner, SNO snoPower)
{


    SNO snoSkillKit = tACDOwner.GetSkillKit();

    const TraitEntry * ptSkillKitEntry = SkillKitGetMatchingTraitEntry(snoSkillKit, snoPower);
    if (ptSkillKitEntry)
    {
        return ptSkillKitEntry->nMasteryReq;
    }

    const ActiveSkillEntry * ptActiveSkillEntry = SkillKitGetMatchingActiveSkillEntry(snoSkillKit, snoPower);
    if (ptActiveSkillEntry)
    {
        return ptActiveSkillEntry->nMasteryReq;
    }

    return 0;
}



static void sGetDamageData(SNO snoPower, ACDID idACDOwner, PowerDamageData & tData)
{


    ScopedPowerDefinitionLockR pdefPower(snoPower);
    SNO snoAlternatePower = PowerGeneralTagGetSNO(pdefPower, TAG_POWER_DAMAGE_DISPLAY_POWER);
    if (snoAlternatePower == SNO_NONE)
    {
        snoAlternatePower = snoPower;
    }

    PowerGetDamageData(AutoPowerDefinitionR(snoAlternatePower), ACDTryToGet(idACDOwner), POWERID_INVALID, tData, NULL, FALSE);
}





static CRefString sParseTipTags(ActorCommonData & tACDOwner,
                                SNO snoPower,
                                int32 nLevelOffset,
                                CRefString usTip)
{
    AttribGroupID idFastAttrib = tACDOwner.AttributesGetAttributeGroup();







    int32 nOldLevel = FastAttribGetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower));
    int32 nNewLevel = nOldLevel + nLevelOffset;
    FastAttribValue nNewValue = nNewLevel;
    CACDAttributeSetValue(tACDOwner, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower), nNewValue, TRUE);

    int32 nReqPlayerLevel = sGetLevelReqForSkill(tACDOwner, snoPower);
    int32 nOldPlayerLevel = tACDOwner.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    int32 nAdjustedPlayerLevel = Math::Max(nReqPlayerLevel, nOldPlayerLevel);
    CACDAttributeSetValue(tACDOwner, FastAttribKey(ATTRIBUTE_LEVEL), nAdjustedPlayerLevel, TRUE);

    usTip = UIToolTips_ProcessTip(&tACDOwner, idFastAttrib, -1, snoPower, nNewLevel, usTip, TTVC_NORMAL_COLORING, UISkills::ResolvePowerTag, NULL, NULL);


    PowerDamageData tDamageData;
    float flDamageMin = 0.0f;
    float flDamageMax = 0.0f;

    sGetDamageData(snoPower, tACDOwner.id, tDamageData);
    for (int32 nDamageType = 0; nDamageType < 7; nDamageType++)
    {
        ElementDamage & tDmg = tDamageData.tElementDamage[nDamageType];
        flDamageMin += tDmg.flTotalMin;
        flDamageMax += tDmg.flTotalMin + tDmg.flTotalDelta;
    }

    CRefString usDamageMin;
    usDamageMin.Format("%d", (int32)flDamageMin);
    CRefString usDamageMax;
    usDamageMax.Format("%d", (int32)flDamageMax);
    usTip = StringListEntryReplace(usTip, "{DamageMin}", usDamageMin, 999);
    usTip = StringListEntryReplace(usTip, "{DamageMax}", usDamageMax, 999);


    CACDAttributeSetValue(tACDOwner, FastAttribKey(ATTRIBUTE_LEVEL), nOldPlayerLevel, TRUE);
    CACDAttributeSetValue(tACDOwner, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower), nOldLevel, TRUE);

    return usTip;
}




CRefString UISkills::GetSkillFlavorText(ActorCommonData & tACDOwner,
                                        const SNO snoPower,
                                        const int32 nLevel,
                                        const BOOL bLongerText)
{
    int32 nLevelOffset =
        (nLevel == 0) ? 1 : 0;






    CRefString sSkillLabel = PowerTrace(snoPower);
    CRefString sSkillTooltipLabel = sGetSkillTooltipLabel(sSkillLabel, bLongerText);
    CRefString sFlavor = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sSkillTooltipLabel);

    CRefString sDesc = sParseTipTags(tACDOwner, snoPower, nLevelOffset, sFlavor);
    CRefString sDescAfterLanguageRules = LanguageRules::ApplyLanguageRules(sDesc.str());
    CRefString sResult = sDescAfterLanguageRules;

    return sResult;
}




static CRefString sGetSkillStatText(ActorCommonData & tACDOwner,
                                    const SNO snoPower,
                                    const int32 nLevel)
{

    int32 nLevelOffset =
        (nLevel == 0) ? 1 : 0;





    CRefString sSkill = PowerTrace(snoPower) + CRefString("_var_stats");



    CRefString usTip;
    if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sSkill, usTip))
    {
        usTip = sParseTipTags(tACDOwner, snoPower, nLevelOffset, usTip);
        usTip += "\n";
        CRefString usAfterLanguageRules = LanguageRules::ApplyLanguageRules(usTip.str());
        return usAfterLanguageRules;
    }


    return "";
}





CRefString UISkills::GetSkillDescriptionText(ActorCommonData & tACDOwner,
                                             const SNO snoPower,
                                             const int32 nLevel,
                                             const BOOL bShowStats,
                                             const BOOL bLongerText)
{
    CRefString sResult;
    if (bShowStats)
    {
        sResult = sGetSkillStatText(tACDOwner, snoPower, nLevel);
        sResult += "\n";
    }

    CRefString sFlavor = GetSkillFlavorText(tACDOwner, snoPower, nLevel, bLongerText);
    Set_Text_Color(sFlavor, FONTCOLOR_GOLD);
    sResult += sFlavor;

    return sResult;
}





SkillCategory UISkills::GetSkillCategoryForSlot(int32 nSlot,
                                                const ACDID idPlayer)
{
    return GetSkillCategoryForSlot(nSlot, PlayerGetPlayerClass(idPlayer));
}





SkillCategory UISkills::GetSkillCategoryForSlot(int32 nSlot,
                                                const PlayerClass ePlayerClass)
{
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        switch (nSlot)
        {
        case 0: return SKILLCAT_DHUNTER_1;
        case 1: return SKILLCAT_DHUNTER_2;
        case 2: return SKILLCAT_DHUNTER_3;
        case 3: return SKILLCAT_DHUNTER_4;
        case 4: return SKILLCAT_DHUNTER_5;
        case 5: return SKILLCAT_DHUNTER_6;
        }
        break;
    case PLAYER_CLASS_BARBARIAN:
        switch (nSlot)
        {
        case 0: return SKILLCAT_BARB_1;
        case 1: return SKILLCAT_BARB_2;
        case 2: return SKILLCAT_BARB_3;
        case 3: return SKILLCAT_BARB_4;
        case 4: return SKILLCAT_BARB_5;
        case 5: return SKILLCAT_BARB_6;
        }
        break;
    case PLAYER_CLASS_WIZARD:
        switch (nSlot)
        {
        case 0: return SKILLCAT_WIZ_1;
        case 1: return SKILLCAT_WIZ_2;
        case 2: return SKILLCAT_WIZ_3;
        case 3: return SKILLCAT_WIZ_4;
        case 4: return SKILLCAT_WIZ_5;
        case 5: return SKILLCAT_WIZ_6;
        }
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        switch (nSlot)
        {
        case 0: return SKILLCAT_WITCHDOC_1;
        case 1: return SKILLCAT_WITCHDOC_2;
        case 2: return SKILLCAT_WITCHDOC_3;
        case 3: return SKILLCAT_WITCHDOC_4;
        case 4: return SKILLCAT_WITCHDOC_5;
        case 5: return SKILLCAT_WITCHDOC_6;
        }
        break;
    case PLAYER_CLASS_MONK:
        switch (nSlot)
        {
        case 0: return SKILLCAT_MONK_1;
        case 1: return SKILLCAT_MONK_2;
        case 2: return SKILLCAT_MONK_3;
        case 3: return SKILLCAT_MONK_4;
        case 4: return SKILLCAT_MONK_5;
        case 5: return SKILLCAT_MONK_6;
        }
        break;
    case PLAYER_CLASS_CRUSADER:
        switch (nSlot)
        {
        case 0: return SKILLCAT_CRUSADER_1;
        case 1: return SKILLCAT_CRUSADER_2;
        case 2: return SKILLCAT_CRUSADER_3;
        case 3: return SKILLCAT_CRUSADER_4;
        case 4: return SKILLCAT_CRUSADER_5;
        case 5: return SKILLCAT_CRUSADER_6;
        }
        break;
    case PLAYER_CLASS_NECROMANCER:
        switch (nSlot)
        {
        case 0: return SKILLCAT_NECROMANCER_1;
        case 1: return SKILLCAT_NECROMANCER_2;
        case 2: return SKILLCAT_NECROMANCER_3;
        case 3: return SKILLCAT_NECROMANCER_4;
        case 4: return SKILLCAT_NECROMANCER_5;
        case 5: return SKILLCAT_NECROMANCER_6;
        }
        break;
    }

    return SKILLCAT_INVALID;
}




int32 UISkills::SkillCategoryGetSlot(SkillCategory eCategory)
{
    switch (eCategory)
    {
    case SKILLCAT_BARB_1:
    case SKILLCAT_WIZ_1:
    case SKILLCAT_WITCHDOC_1:
    case SKILLCAT_DHUNTER_1:
    case SKILLCAT_MONK_1:
    case SKILLCAT_CRUSADER_1:
    case SKILLCAT_NECROMANCER_1:
        return 0;
    case SKILLCAT_BARB_2:
    case SKILLCAT_WIZ_2:
    case SKILLCAT_WITCHDOC_2:
    case SKILLCAT_DHUNTER_2:
    case SKILLCAT_MONK_2:
    case SKILLCAT_CRUSADER_2:
    case SKILLCAT_NECROMANCER_2:
        return 1;
    case SKILLCAT_BARB_3:
    case SKILLCAT_WIZ_3:
    case SKILLCAT_WITCHDOC_3:
    case SKILLCAT_DHUNTER_3:
    case SKILLCAT_MONK_3:
    case SKILLCAT_CRUSADER_3:
    case SKILLCAT_NECROMANCER_3:
        return 2;
    case SKILLCAT_BARB_4:
    case SKILLCAT_WIZ_4:
    case SKILLCAT_WITCHDOC_4:
    case SKILLCAT_DHUNTER_4:
    case SKILLCAT_MONK_4:
    case SKILLCAT_CRUSADER_4:
    case SKILLCAT_NECROMANCER_4:
        return 3;
    case SKILLCAT_BARB_5:
    case SKILLCAT_WIZ_5:
    case SKILLCAT_WITCHDOC_5:
    case SKILLCAT_DHUNTER_5:
    case SKILLCAT_MONK_5:
    case SKILLCAT_CRUSADER_5:
    case SKILLCAT_NECROMANCER_5:
        return 4;
    case SKILLCAT_BARB_6:
    case SKILLCAT_WIZ_6:
    case SKILLCAT_WITCHDOC_6:
    case SKILLCAT_DHUNTER_6:
    case SKILLCAT_MONK_6:
    case SKILLCAT_CRUSADER_6:
    case SKILLCAT_NECROMANCER_6:
        return 5;
    }











    return -1;
}



CRefString UISkills::GetSkillCategoryLabel(const SkillCategory eCategory)
{
    CRefString sLabel("Cat_");
    switch (eCategory)
    {
    case SKILLCAT_BARB_1: sLabel += "Barb1"; break;
    case SKILLCAT_BARB_2: sLabel += "Barb2"; break;
    case SKILLCAT_BARB_3: sLabel += "Barb3"; break;
    case SKILLCAT_BARB_4: sLabel += "Barb4"; break;
    case SKILLCAT_BARB_5: sLabel += "Barb5"; break;
    case SKILLCAT_BARB_6: sLabel += "Barb6"; break;
    case SKILLCAT_WIZ_1: sLabel += "Wiz1"; break;
    case SKILLCAT_WIZ_2: sLabel += "Wiz2"; break;
    case SKILLCAT_WIZ_3: sLabel += "Wiz3"; break;
    case SKILLCAT_WIZ_4: sLabel += "Wiz4"; break;
    case SKILLCAT_WIZ_5: sLabel += "Wiz5"; break;
    case SKILLCAT_WIZ_6: sLabel += "Wiz6"; break;
    case SKILLCAT_WITCHDOC_1: sLabel += "WitchDoc1"; break;
    case SKILLCAT_WITCHDOC_2: sLabel += "WitchDoc2"; break;
    case SKILLCAT_WITCHDOC_3: sLabel += "WitchDoc3"; break;
    case SKILLCAT_WITCHDOC_4: sLabel += "WitchDoc4"; break;
    case SKILLCAT_WITCHDOC_5: sLabel += "WitchDoc5"; break;
    case SKILLCAT_WITCHDOC_6: sLabel += "WitchDoc6"; break;
    case SKILLCAT_DHUNTER_1: sLabel += "DHunter1"; break;
    case SKILLCAT_DHUNTER_2: sLabel += "DHunter2"; break;
    case SKILLCAT_DHUNTER_3: sLabel += "DHunter3"; break;
    case SKILLCAT_DHUNTER_4: sLabel += "DHunter4"; break;
    case SKILLCAT_DHUNTER_5: sLabel += "DHunter5"; break;
    case SKILLCAT_DHUNTER_6: sLabel += "DHunter6"; break;
    case SKILLCAT_MONK_1: sLabel += "Monk1"; break;
    case SKILLCAT_MONK_2: sLabel += "Monk2"; break;
    case SKILLCAT_MONK_3: sLabel += "Monk3"; break;
    case SKILLCAT_MONK_4: sLabel += "Monk4"; break;
    case SKILLCAT_MONK_5: sLabel += "Monk5"; break;
    case SKILLCAT_MONK_6: sLabel += "Monk6"; break;
    case SKILLCAT_CRUSADER_1: sLabel += "Crusader1"; break;
    case SKILLCAT_CRUSADER_2: sLabel += "Crusader2"; break;
    case SKILLCAT_CRUSADER_3: sLabel += "Crusader3"; break;
    case SKILLCAT_CRUSADER_4: sLabel += "Crusader4"; break;
    case SKILLCAT_CRUSADER_5: sLabel += "Crusader5"; break;
    case SKILLCAT_CRUSADER_6: sLabel += "Crusader6"; break;
    case SKILLCAT_NECROMANCER_1: sLabel += "Necromancer1"; break;
    case SKILLCAT_NECROMANCER_2: sLabel += "Necromancer2"; break;
    case SKILLCAT_NECROMANCER_3: sLabel += "Necromancer3"; break;
    case SKILLCAT_NECROMANCER_4: sLabel += "Necromancer4"; break;
    case SKILLCAT_NECROMANCER_5: sLabel += "Necromancer5"; break;
    case SKILLCAT_NECROMANCER_6: sLabel += "Necromancer6"; break;
    default:


        XASSERT(FALSE);
    }

    return sLabel;
}
