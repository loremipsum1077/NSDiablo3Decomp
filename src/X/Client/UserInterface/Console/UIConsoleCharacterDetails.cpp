#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../CGame.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIBuffs.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../ImageCollection.h"
#include "../../../../Contrib/Contrib/blz/include/blz/iter_range.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"  // blz::ignore: the original reaches it through an include chain not adopted here
typedef int32 DamageType; void PowerGetBuffAttributes(int32, Attrib &, Attrib &, Attrib &); float PowerComputeResistanceDamageMitigation(float flResistanceTotal, int32 nAttackerLevel); extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS_LIST; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL_SCROLL_BOX; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_SCROLL_BOX; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_TEXT; extern UIID UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL_NO_BUFFS_TEXT;  // not adopted yet: PowerGetBuffAttributes (X/Common/Powers/PowersCommon.h), UIIDs.h
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id); UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  /* UI.cpp */ namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta); } } namespace UIBuffs { CRefString GetBuffTooltipText(const UIBuffs::SortedBuff & tBuff); CRefString GetBuffName(SNO snoPower, int32 nBuffIndex); }  // UIConsoleUtils.cpp, UIBuffs.cpp
namespace Console { namespace Screens { typedef int32 GameplayScreens; void OpenToScreen(int32 nScreenSet, int32 nScreen, int32 nControllerIndex, BOOL bUnused); } }  // ConsoleScreenManager.cpp
bool InputIsSingleJoycon(int32_t nControllerIndex);  // InputNX64.cpp
extern "C" BOOL sub_7100527370(const ActorCommonData & tACD);  // Player.cpp region: returns AttributesGetInt(ATTRIBUTE_PARAGON_CAP_ENABLED)
namespace UICharacterDetails {
namespace Console {




static float sflRightStickScrollAmount = 0.0f;

enum ButtonActions {
    ACTION_SCROLL_DETAILS_LIST = 0,
    ACTION_SCROLL_BUFFS_LIST = 1,
    ACTION_SCROLL_DETAIL_INFO = 2,
    ACTION_CLOSE = 3,
};







struct StatListEntry {
    int32 nIndex;
    CRefString strDesc;
    CRefString strValue;
    CRefString strTip;


    StatListEntry() { nIndex = -1; strDesc = ""; strValue = ""; strTip = ""; }
};

typedef blz::vector<StatListEntry, blz::allocator<StatListEntry> > StatList;







class CharacterStatsDataSource : public ConsoleListBoxDataSource {
public:
    CharacterStatsDataSource();
    const StatListEntry & GetItem(const int32) const;
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);
    void PopulateInfoList(const Player * ptPlayer);
    void UpdateStatDetails(const UIID & id, const int32 nOldVal, const int32 nNewVal);
    void Clear();
private:
    StatList m_StatList;
    const Player * m_ptPlayer;
    void AddStat(const CRefString & strStatDesc, const CRefString & strValue, const CRefString & strTip);
    void AddCategoryTitle(const CRefString & strTitle);
};
} // Close UICharacterDetails::Console for original external declarations.
}
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/ActorCommonDataSwig.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Sigma/DataStructures/XPtr64.h"
// DWARF UIInventoryCommon.h:22; members24-28, size0x14, offsets0/4/8/12/16.
// Inference: local declaration placement preserves the original external namespace.
namespace UIInventory {
struct HeroModifierArray {
    float flBase;
    float flTotal;
    float flWeapons;
    float flArmor;
    float flJewelry;
};
// DWARF UIInventoryCommon.cpp:128 and515.
void GetHeroModifiers(ActorCommonData & tACD, FastAttribKey tKey, FastAttribKey tKeyTotal, HeroModifierArray & tDestArray, const BOOL fIncludeAllStatsBonus);
CRefString GetDPSString(float flDPS);
}
// DWARF X/Common/UserInterface/UISkillsCommon.cpp:21.
namespace UISkills { BOOL GetName(SNO snoPower, CRefString & sSkillName); }
// DWARF X/Common/Hireling.cpp:900.
float HirelingGetDiminishedAttribute(ACDID idACDPlayer, Attrib eAttrib);
// DWARF X/Common/ExperienceCommon.cpp:20 and137.
uint64 ExperienceGetCurrentLevelXP(const ActorCommonData & tACD, const BOOL bUseAlt);
float ExperienceGetBonusFromGear(const ActorCommonData & tACDPlayer);
#include "../../../Common/Powers/PowerAPISwig.h"
namespace UICharacterDetails {
namespace Console {








inline CharacterStatsDataSource::CharacterStatsDataSource() : m_ptPlayer(NULL)
{
}





int32 CharacterStatsDataSource::GetNumItems() const
{
    return m_StatList.size();
}




UIControlSourceKey CharacterStatsDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleCharacterDetails:ConsoleCharacterDetails_main.LayoutRoot.DetailsList.DetailInfo", SNO_NONE, NULL, NULL);
}




void CharacterStatsDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}





inline void CharacterStatsDataSource::Clear()
{

    m_StatList.clear();
}



void CharacterStatsDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idDesc;
    idDesc.SetFromParent(idItemUI, "Name");
    UIText & desc = UI_GetTextFromID(idDesc);




    desc.SetText(m_StatList[nIndex].strDesc, FALSE);
    UIID idValue;
    idValue.SetFromParent(idItemUI, "Amount");
    UIText & value = UI_GetTextFromID(idValue);
    value.SetText(m_StatList[nIndex].strValue, FALSE);
    UIID idSelected;
    idSelected.SetFromParent(idItemUI, "Selected");
    UIWindow & selected = UI_GetWindowFromID(idSelected);
    selected.SetActive(tListBox.GetSelectedIndex() == nIndex);
}




BOOL CharacterStatsDataSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{


    return m_StatList[nIndex].strTip != "" ||
        (tListBox.GetFirstVisibleItemIndex() == 1 && nIndex == 0);
}



void CharacterStatsDataSource::UpdateStatDetails(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{



    if (nNewVal < 0 || nNewVal >= (int32)m_StatList.size())
    {
        return;
    }

    const StatListEntry & tListEntry = m_StatList[nNewVal];
    if (tListEntry.strTip == "")
    {
        UIConsoleListBox & listBox = UI_GetConsoleListBoxFromID(id);

        if (nNewVal > nOldVal && nNewVal + 1 < (int32)m_StatList.size())
        {
            listBox.SelectNextItem(FALSE);
        }
        else if (nNewVal < nOldVal && nNewVal > 0)
        {
            listBox.SelectPreviousItem(FALSE);
        }
        else if (nNewVal == 0)
        {
            listBox.SelectNextItem(FALSE);
        }
    }
    else
    {
        UI_GetConsoleScrollBoxFromID(UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_SCROLL_BOX).SetScrollPos(0.0f);
        UIText & tDetailText = UI_GetTextFromID(UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_TEXT);
        tDetailText.SetText(tListEntry.strTip, FALSE);
    }
}






CRefString GetBaseStatString(
    ActorCommonData & tACD,
    Attrib eAttribute)
{
    UIInventory::HeroModifierArray tStatArray;
    UIInventory::GetHeroModifiers(tACD, eAttribute, eAttribute, tStatArray, TRUE);
    CRefString usStatValue = UnicodeText::FormatNumber(tStatArray.flTotal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);

    if (tStatArray.flTotal - tStatArray.flBase > 0.000001f)
    {
        Set_Text_Color(usStatValue, FONTCOLOR_BONUS);
    }
    else if (tStatArray.flBase - tStatArray.flTotal > 0.000001f)
    {
        Set_Text_Color(usStatValue, FONTCOLOR_ORANGE);
    }

    // Inference: returning the named string elides its copy into each caller result.
    return usStatValue;
}


CRefString GetResistanceString(
    const ActorCommonData & tACD,
    int32 nParam)
{
    float flVal = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_IMMUNITY, nParam)) ? 999.0f : tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, nParam));
    return UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
}

CRefString GetResistanceTooltipString(
    const ActorCommonData & tACD,
    DamageType eDamageType,
    const LPCSTR szLabel)
{
    float flResistanceTotal = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, eDamageType));
    int32 nCurrentLevel = tACD.AttributesGetInt(ATTRIBUTE_EFFECTIVE_LEVEL);
    CRefString usResistanceTotal = UnicodeText::FormatNumber(flResistanceTotal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString usLevel = UnicodeText::FormatNumber(nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    float flVal = PowerComputeResistanceDamageMitigation(flResistanceTotal, nCurrentLevel);
    CRefString usStat = UnicodeText::FormatNumber(flVal, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

    CRefString usTip;

    float flBonusReduction = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, eDamageType));
    if (flBonusReduction > 0.000001f)
    {
        CRefString sLabelBonus(szLabel);
        sLabelBonus += "Bonus";
        CRefString usBonusPercent = UnicodeText::FormatNumber(flBonusReduction, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), sLabelBonus.str()),
            usResistanceTotal.str(), usLevel.str(), usStat.str(), usBonusPercent.str(), NULL);
    }
    else
    {
        usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), szLabel),
            usResistanceTotal.str(), usLevel.str(), usStat.str(), NULL);
    }

    return usTip;
}



void CharacterStatsDataSource::AddStat(const CRefString & strStatDesc, const CRefString & strValue, const CRefString & strTip)
{
    StatListEntry tEntry;
    tEntry.strDesc = strStatDesc;
    tEntry.strValue = strValue;
    tEntry.strTip = strTip;
    m_StatList.push_back(tEntry);
}







void CharacterStatsDataSource::AddCategoryTitle(const CRefString & strTitle)
{
    StatListEntry tEntry;
    tEntry.strDesc.Format("{c_yellow}%s{/c}", strTitle.str());
    tEntry.strValue = "";
    m_StatList.push_back(tEntry);
}





// NON_MATCHING: 15728 bytes both; 7 instructions differ only by a w23/w24 register swap (snoPower vs hoisted "TipSkillBonus" address) in the power-bonus loop (lines 614-628); tried a dead nPowerBonusIdx++ (no change).

void CharacterStatsDataSource::PopulateInfoList(
    const Player * ptPlayer)
{
    m_ptPlayer = ptPlayer;
    ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
    int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_EFFECTIVE_LEVEL);

    AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "PrimaryAttributes"));



    {
        CRefString usStatValue = GetBaseStatString(tACD, ATTRIBUTE_STRENGTH_TOTAL);

        float flVal = tACD.AttributesGetFloat(ATTRIBUTE_STRENGTH_TOTAL);
        float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usArmorBonus = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);

        CRefString usStatTooltip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipStrength"),
            usVal.str(), usValBonus.str(), usArmorBonus.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "Strength"), usStatValue, usStatTooltip);


    }



    {
        CRefString usStatValue = GetBaseStatString(tACD, ATTRIBUTE_DEXTERITY_TOTAL);

        float flVal = tACD.AttributesGetFloat(ATTRIBUTE_DEXTERITY_TOTAL);
        float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        CRefString usStatTooltip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipDexterity"),
            usVal.str(), usValBonus.str(), usVal.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "Dexterity"), usStatValue, usStatTooltip);


    }



    {
        CRefString usStatValue = GetBaseStatString(tACD, ATTRIBUTE_INTELLIGENCE_TOTAL);

        float flVal = tACD.AttributesGetFloat(ATTRIBUTE_INTELLIGENCE_TOTAL);
        float flValBonus = PowerComputeDamageScalarFromPrimaryAttributeValue(flVal);
        float flResistBonus = tACD.AttributesGetFloat(ATTRIBUTE_RESIST_FROM_INTELLIGENCE);
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usValBonus = UnicodeText::FormatNumber(flValBonus, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
        CRefString usResistanceBonus = UnicodeText::FormatNumber(flResistBonus, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID);

        CRefString usStatTooltip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipIntelligence"),
            usVal.str(), usValBonus.str(), usResistanceBonus.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "Intelligence"), usStatValue, usStatTooltip);


    }



    {
        CRefString usStatValue = GetBaseStatString(tACD, ATTRIBUTE_VITALITY_TOTAL);

        float flVal = tACD.AttributesGetFloat(ATTRIBUTE_VITALITY_TOTAL);
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_FACTOR_VITALITY);
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);

        CRefString usStatTooltip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipVitality"),
            usVal.str(), usStat.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "Vitality"), usStatValue, usStatTooltip);


    }


    {
        int32 nArmor = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_ARMOR_TOTAL));
        CRefString sArmor = UnicodeText::FormatNumber(nArmor, TRUE, FNS_NORMAL, XLOCALE_INVALID);

        float flVal = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_ARMOR_TOTAL));
        float flStat = PowerComputeArmorDamageMitigation(tACD, nLevel);
        CRefString usVal = UnicodeText::FormatNumber(flVal, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usLevel = UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipArmor"),
            usVal.str(), usLevel.str(), usStat.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "Armor"), sArmor, usTip);


    }

    {
        float flDPS = tACD.GetDPSValue(ACDIsDualWielding(tACD, TRUE));
        CRefString sDPS = UnicodeText::FormatNumber(flDPS, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipDPS"), sDPS.str(), NULL);


        CRefString usStatName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "DamagePerSecond");
        AddStat(usStatName, sDPS, usTip);
    }



    AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CategoryOffense"));


    {
        float flStat = PowerComputeDamageScalarFromPrimaryAttribute(tACD.idFastAttrib);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        CRefString usOffenseBonusNameText;
        CRefString usTipOffenseBonusNameText;
        PrimaryDamageAttribute ePrimaryDamageAttribute = tACD.AttributesGetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE);
        switch (ePrimaryDamageAttribute)
        {
        case PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH:
            usOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DamageIncreaseStrength");
            usTipOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageBonusStrength");
            break;
        case PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY:
            usOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DamageIncreaseDexterity");
            usTipOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageBonusDexterity");
            break;
        case PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE:
            usOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DamageIncreaseIntelligence");
            usTipOffenseBonusNameText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageBonusIntelligence");
            break;
        }





        AddStat(usOffenseBonusNameText, usStat, LocalizedSprintf(usTipOffenseBonusNameText, usStat.str(), NULL));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DamageIncreaseSkills"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageBonusSkills"), usStat.str(), NULL));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "BonusDamageElites"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusDamageElites"), usStat.str(), NULL));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "AttacksPerSecond"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeed"), usStat.str(), NULL));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_SUBTOTAL) - 1.0f;
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "AttackSpeedIncrease"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipAttackSpeedPercent"), usStat.str(), NULL));
    }

    {
        float flStat = PowerComputeCritChance(&tACD, FALSE);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        float flStat2 = PowerComputeCritDamageScalar(&tACD) - 1.0f;
        CRefString usStat2 = CRefString(flStat2 >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat2, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CritChanceBonus"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCritChance"), usStat.str(), usStat2.str(), NULL));
    }
    {
        float flStat = PowerComputeCritDamageScalar(&tACD) - 1.0f;
        CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CritDamageBonus"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCritDamageBonus"), usStat.str(), NULL));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        ScopedGlobalDefinitionLockR pdefGlobal;
        float flSplashChance = pdefGlobal->flSplashDamageEffectProcChance;
        CRefString usChance = UnicodeText::FormatNumber(flSplashChance, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
        float flSplashRadius = pdefGlobal->flSplashDamageEffectRadius;
        CRefString usRadius = UnicodeText::FormatNumber(flSplashRadius, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

        CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSplashDamage"),
            usStat.str(),
            usChance.str(),
            usStat.str(),
            usRadius.str(), NULL);


        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "SplashDamage"), usStat, usTip.str());


    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_PERCENT_ALL_CAPPED);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CooldownReduction"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCooldownReduction"), usStat.str(), NULL));
    }
    // Inference: DWARF lexical block lines 577-635 holds nDamageType loop, nPowerBonusIdx and tIt; the block scope makes tIt's stack slot reusable afterwards (frame 0x590).
    {
    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
    {
        float flStat = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, nDamageType));
        if (flStat > 0.000001f)
        {
            CRefString sDamageTypeLabel;
            sDamageTypeLabel.Format("DamageTypeBonus%s", SymbolToName(nDamageType, gDamageTypes));

            CRefString sDamageType;
            CRefString sTooltipLabel = CRefString().Format("Damage_%s", SymbolToName(nDamageType, gDamageTypes));
            sDamageType = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), sTooltipLabel.str());
            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
            BOOL bWasDualWielding = ACDIsDualWielding(tACD, TRUE);
            float flVal = tACD.GetDPSValue(bWasDualWielding);
            flVal *= 1.0f + flStat;
            CRefString usDPS = UIInventory::GetDPSString(flVal);
            CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageTypeBonus"),
                sDamageType.str(), usStat.str(), sDamageType.str(), usDPS.str());

            AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sDamageTypeLabel), usStat, usTip);

        }
    }

    // DWARF602 POWER_BONUS_UI_MAX is optimized away without a recoverable value; no initializer invented.
    // Omitted unused const int32 POWER_BONUS_UI_MAX (DWARF DIE0xca724c).
    int32 nPowerBonusIdx = 0;
    FastAttribIterator tIt(tACD.idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
        Attrib eAttribute = KeyGetAttrib(tKey);
        if (eAttribute == ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS)
        {
            float flStat = tACD.AttributesGetFloat(tKey);
            if (flStat > 0.000001f)
            {
                SNO snoPower = (SNO)KeyGetParam(tKey);
                PlayerClass ePlayerClass = PlayerGetPlayerClass(tACD.id);
                PlayerClass eClassRestriction = PowerGetClassRestriction(snoPower);
                if (eClassRestriction != PLAYER_CLASS_NONE && ePlayerClass != eClassRestriction)
                {
                    continue;
                }

                CRefString strPowerName;
                if (UISkills::GetName(snoPower, strPowerName))
                {
                    CRefString usName = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "SkillBonus"),
                        strPowerName.str(), NULL);
                    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);
                    CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSkillBonus"),
                        strPowerName.str(), usStat.str(), NULL);

                    AddStat(usName, usStat, usTip);


                }
            }

        }
    }
    } // Inference: end of the DWARF block 577-635 (closing-brace line not in the line table).
    AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CategoryDefenses"));

    {
        float flStatMin = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MIN);
        float flStatMax = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MAX);
        CRefString sStatMin = UnicodeText::FormatNumber(flStatMin, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString sStatMax = UnicodeText::FormatNumber(flStatMax, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
        CRefString usStat = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RangeFormat"),
            sStatMin.str(), sStatMax.str(), NULL);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "BlockAmount"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBlockAmount"), sStatMin.str(), sStatMax.str(), NULL));


        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_BLOCK_CHANCE_CAPPED_TOTAL);
        CRefString usStat2 = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "BlockChance"),
            usStat2,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBlockChance"), usStat2.str(), sStatMin.str(), sStatMax.str(), NULL));
    }
    {
        float flStat = Math::Max(PowerComputeDodgeChance(tACD, nLevel, PDM_MELEE, FALSE), 0.0f);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DodgeChance"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDodgeChance"), usStat.str(), NULL));
    }

    {
        float flStat = PowerComputeArmorDamageMitigation(tACD, nLevel);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DamageReduction"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDamageReduction"), usStat.str(), usStat.str(), NULL));
    }

    CRefString usLevelString = UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_PHYSICAL);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "PhysicalResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_PHYSICAL, "TipPhysicalResist"));
    }

    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_COLD);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "ColdResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_COLD, "TipColdResist"));
    }
    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_FIRE);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "FireResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_FIRE, "TipFireResist"));
    }
    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_LIGHTNING);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LightningResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_LIGHTNING, "TipLightningResist"));
    }
    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_POISON);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "PoisonResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_POISON, "TipPoisonResist"));
    }
    {
        CRefString usResistance = GetResistanceString(tACD, DAMAGE_TYPE_ARCANE);
        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "ArcaneResistance"),
            usResistance,
            GetResistanceTooltipString(tACD, DAMAGE_TYPE_ARCANE, "TipArcaneResist"));
    }

    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_REDUCTION);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CCReduction"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipCCReduction"), usStat.str(), NULL));
    }
    {
        float flStat = tACD.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED);
        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "MissileDefenseBonus"),
            usStat,
            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMissileDefenseBonus"), usStat.str(), NULL));
    }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE);
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "MeleeDefenseBonus"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMeleeDefenseBonus"), usStat.str(), NULL));
            }
            { // Inference: initial add to zero and seven unrolled iterations match the sibling thorns loop.
                float flStat = 0.0f;
                for (int eDamageType = 0; eDamageType < NUM_DAMAGE_TYPES; eDamageType++)
                {
                    flStat += tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_THORNS_FIXED, eDamageType));
                }
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "ThornsFixed"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipThornsFixed"), usStat.str(), NULL));
            }

            AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CategoryLife"));

            {
                int32 nValMax = Math::FloatRoundToInt(tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
                CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "Life"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLife"), "", usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS) + tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS_ITEM);
                CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LifeFromAffix"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeFromAffix"), usStat.str(), usStat.str(), NULL));
            }
            // Inference: original adds the bonus to a persistent 1.0f before subtracting reduced healing.
            float flHealingModifier = 1.0f;
            flHealingModifier += tACD.AttributesGetFloat(ATTRIBUTE_BONUS_HEALING_RECEIVED_PERCENT);
            flHealingModifier -= tACD.AttributesGetFloat(ATTRIBUTE_REDUCED_HEALING_RECEIVED_PERCENT);



            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_SUBTOTAL);
                flStat += tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS_FROM_SKILLS);
                flStat *= flHealingModifier;
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LifePerSecond"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeRegen"), usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT);
                flStat *= flHealingModifier;
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LifeSteal"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifeSteal"), usStat.str(), usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_KILL);
                flStat *= flHealingModifier;
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LifePerKill"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerKill"), usStat.str(), usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT);
                flStat *= flHealingModifier;
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "LifePerHit"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipLifePerHit"), usStat.str(), usStat.str(), NULL));
            }

            {
                ResourceType eResourceType = tACD.GetPrimaryResourceType();
                switch (eResourceType)
                {
                case RESOURCE_TYPE_FURY:
                case RESOURCE_TYPE_SPIRIT:
                case RESOURCE_TYPE_FAITH:
                    {
                        CRefString sLabel = CRefString().Format("LifeOn%sSpent", SymbolToName(eResourceType, gResourceTypes));
                        CRefString sTipLabel = CRefString().Format("TipLifePer%sSpent", SymbolToName(eResourceType, gResourceTypes));
                        CRefString sResourceName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel);

                        float flHealPercent = tACD.ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, eResourceType);
                        flHealPercent *= flHealingModifier;
                        CRefString usStat = UnicodeText::FormatNumber(flHealPercent, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                        AddStat(sResourceName,
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sTipLabel.str()), usStat.str(), usStat.str(), NULL));
                    }
                    break;
                }






                // Original calls FormatNumber then destroys usStat without adding a displayed row (848-852).
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT);
                flStat *= flHealingModifier;
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

            }

            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH);
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "HealthGlobeBonus"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipHealthGlobeBonus"), usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_GOLD_PICKUP_RADIUS);
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "GlobeRadiusBonus"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipGlobeRadiusBonus"), usStat.str(), NULL));
            }

            AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CategoryResource"));


            {
                ResourceType eResourceType = tACD.GetPrimaryResourceType();

                {
                    int32 nValMax = Math::FloatFloorToInt(tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType));
                    CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);
                    CRefString sLabel = CRefString().Format("Max%s", SymbolToName(eResourceType, gResourceTypes));

                    AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel),
                        usStat,
                        LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), CRefString().Format("TipMax%s", SymbolToName(eResourceType, gResourceTypes))), usStat.str(), NULL));

                }

                switch (eResourceType)
                {
                case RESOURCE_TYPE_MANA:
                    {
                        float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_MANA);
                        float flRegenGainPct = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_MANA);
                        float flStat = flRegenPerSecond * (1.0f + flRegenGainPct) * flHealingModifier;

                        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

                        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "ManaGain"),
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipManaGain"), usStat.str(), NULL));
                    }
                    break;
                case RESOURCE_TYPE_ARCANUM:
                    {
                        float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_ARCANUM);
                        float flRegenGainPct = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_ARCANUM);
                        float flStat = flRegenPerSecond * (1.0f + flRegenGainPct);

                        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

                        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "ArcanumGain"),
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipArcanumGain"), usStat.str(), NULL));
                    }
                    break;
                case RESOURCE_TYPE_HATRED:
                    {
                        float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_HATRED);
                        float flRegenGainPct = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_HATRED);
                        float flStat = flRegenPerSecond * (1.0f + flRegenGainPct);

                        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

                        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "HatredGain"),
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipHatredGain"), usStat.str(), NULL));
                    }
                    break;
                case RESOURCE_TYPE_SPIRIT:
                    {
                        float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_SPIRIT);
                        float flRegenGainPct = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_SPIRIT);
                        float flStat = flRegenPerSecond * (1.0f + flRegenGainPct);

                        CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

                        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "SpiritGain"),
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipSpiritGain"), usStat.str(), NULL));
                    }
                    break;






                }

                {
                    float flCostReduction = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_TOTAL, eResourceType); // Inference: following DWARF flStat aliases this unchanged value.
                    float flStat = flCostReduction;

                    CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
                    CRefString sLabel = CRefString().Format("%s_Cost_Reduction", SymbolToName(eResourceType, gResourceTypes));
                    CRefString sTipLabel = CRefString().Format("Tip%sCostReduction", SymbolToName(eResourceType, gResourceTypes));

                    AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel),
                        usStat,
                        LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sTipLabel), usStat.str(), NULL));
                }

                ResourceType eSecondaryResourceType = tACD.GetSecondaryResourceType();
                if (eSecondaryResourceType != RESOURCE_TYPE_INVALID)
                {
                    {
                        int32 nValMax = Math::FloatFloorToInt(tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eSecondaryResourceType));
                        CRefString usStat = UnicodeText::FormatNumber(nValMax, TRUE, FNS_NORMAL, XLOCALE_INVALID);
                        CRefString sLabel = CRefString().Format("Max%s", SymbolToName(eSecondaryResourceType, gResourceTypes));

                        AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel),
                            usStat,
                            LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), CRefString().Format("TipMax%s", SymbolToName(eSecondaryResourceType, gResourceTypes))), usStat.str(), NULL));
                    }
                    // Inference: a singleton resource switch spells the original comparison against DISCIPLINE.
                    switch (eSecondaryResourceType)
                    {
                    case RESOURCE_TYPE_DISCIPLINE:

                        {
                            float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, eSecondaryResourceType); // Inference: following DWARF flStat aliases this unchanged value.
                            float flStat = flRegenPerSecond;
                            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 0, 2, XLOCALE_INVALID);

                            AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "DisciplineGain"),
                                usStat,
                                LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipDisciplineGain"), usStat.str(), NULL));
                        }


                        {
                            float flCostReduction = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_TOTAL, eSecondaryResourceType); // Inference: following DWARF flStat aliases this unchanged value.
                            float flStat = flCostReduction;

                            CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
                            CRefString sLabel = CRefString().Format("%s_Cost_Reduction", SymbolToName(eSecondaryResourceType, gResourceTypes));
                            CRefString sTipLabel = CRefString().Format("Tip%sCostReduction", SymbolToName(eSecondaryResourceType, gResourceTypes));

                            AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sLabel),
                                usStat,
                                LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), sTipLabel), usStat.str(), NULL));
                        }
                        break;
                    }
                }
            }


            AddCategoryTitle(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "CategoryAdventurer"));

            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_MOVEMENT_SCALAR_TOTAL) - 1.0f;
                CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
                float flStatPlus100 = flStat + 1.0f;
                CRefString usStatPlus100 = UnicodeText::FormatNumber(flStatPlus100, TRUE, FNS_PERCENT, 0, 2, XLOCALE_INVALID);
                LPCSTR szTip = sub_7100527370(tACD) ? "TipMovementSpeedParagonCap" : "TipMovementSpeed";

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "MovementSpeed"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), szTip), usStat.str(), usStatPlus100.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_GOLD_FIND_TOTAL);


                flStat += HirelingGetDiminishedAttribute(tACD.id, ATTRIBUTE_GOLD_FIND_TOTAL);

                CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "GoldFind"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipGoldFind"), usStat.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_TOTAL);


                flStat += HirelingGetDiminishedAttribute(tACD.id, ATTRIBUTE_MAGIC_FIND_TOTAL);

                CRefString usStat = CRefString(flStat >= 0.0f ? "+" : "") + UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "MagicFind"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipMagicFind"), usStat.str(), NULL));
            }
            {
                BOOL bUseAlt = tACD.ShouldUseAltLevels();
                uint64 nXPCurrent = ExperienceGetCurrentLevelXP(tACD, bUseAlt);
                uint64 nXPRemainingToLevel = bUseAlt ? tACD.GetAltExperienceNext() : tACD.GetExperienceNext();

                CRefString usStatCurrent = UnicodeText::FormatNumber(nXPCurrent, TRUE, FNS_NORMAL, XLOCALE_INVALID);
                CRefString usStatNext = UnicodeText::FormatNumber(nXPRemainingToLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);

                CRefString usTip = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO), "TipExperience"),
                    usStatCurrent.str(), usStatNext.str(), NULL);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_H2O_LAYOUT), "Experience"), usStatCurrent, usTip);



            }
            {
                float flStat = ExperienceGetBonusFromGear(tACD);
                flStat += HirelingGetDiminishedAttribute(tACD.id, ATTRIBUTE_EXPERIENCE_BONUS_PCT_TOTAL);

                float flStat2 = tACD.AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS_PCT_SUPER_SCALAR_TOTAL);

                float flStatTotal = (flStat * flStat2) - 1.0f;

                ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS)); // Inference: original table-base load selects the first entry below.
                const TieredLootRunLevel &tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[0];
                float flStat3 = 1.0f - tTieredLootRunLevel.tLootRunValues.flXPReductionScalar;

                CRefString usTotal = UnicodeText::FormatNumber(flStatTotal, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
                CRefString usStat2 = UnicodeText::FormatNumber(flStat2, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);
                CRefString usStat3 = UnicodeText::FormatNumber(flStat3, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "BonusXP"),
                    usTotal,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusXP"), usTotal.str(), usStat.str(), usStat2.str(), usStat3.str(), NULL));
            }
            {
                float flStat = tACD.AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS);

                flStat += tACD.AttributesGetFloat(ATTRIBUTE_EXPERIENCE_BONUS_NO_PENALTY);


                flStat += HirelingGetDiminishedAttribute(tACD.id, ATTRIBUTE_EXPERIENCE_BONUS);

                CRefString usStat = UnicodeText::FormatNumber(flStat, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID);

                AddStat(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "BonusXPPerKill"),
                    usStat,
                    LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_HERO_DETAILS), "TipBonusXPPerKill"), usStat.str(), usStat.str(), NULL));
            }
}



struct BuffData {
    UIID idBuff;
    UIID idBuffText;
    UIBuffs::SortedBuff tBuff;
};

typedef blz::vector<BuffData, blz::allocator<BuffData> > BuffList;


static BuffList stBuffList;

static int32 snOpeningControllerIndex = -1;
static int32 snSelectedIndex = 0;
static int32 snCount = 0;
static const Player * sptPlayer = NULL;
static uint32 snLastUpdateTime = 0;
static ::Console::Screens::GameplayScreens sePrevScreen = 1;
static CharacterStatsDataSource stStatsDataSource;

void UpdateButtonLegend();
void DisplayBuffs(const Player * ptPlayer); void HandleListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);





void InitializeForGame()
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS_LIST);
    tListBox.SetDataSource(stStatsDataSource);
    tListBox.SetOnSelChangeFunction(HandleListSelectionChanged);
}





void DisposeForGame()
{
}




void AddBuffsToDisplay(
    const Player * ptPlayer,
    const blz::vector<const UIBuffs::SortedBuff *, blz::allocator<const UIBuffs::SortedBuff *> > & vecBuffs,
    const bool bIsBeneficial)
{
    UIStackPanel & tBuffPanel = UI_GetStackPanelFromID(UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL);
    for (const UIBuffs::SortedBuff * ptBuff : vecBuffs)
    {
        ActorCommonData * ptACDAffected = ACDTryToGet(ptBuff->ptPlayer->idPlayerACD);
        if (ptACDAffected)
        {
            Attrib eAttribStart;
            Attrib eAttribEnd;
            Attrib eAttribCount;



            PowerGetBuffAttributes(ptBuff->nBuffIdx, eAttribStart, eAttribEnd, eAttribCount);

            uint32 dwEndTick = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribEnd, ptBuff->snoPower));
            uint32 dwNow = CGameGetPredictedServerTime();

            int32 nStackCount = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribCount, ptBuff->snoPower));
            BOOL bInfinite = !ptACDAffected->AttributesAttributeExists(MakeAttribKey(eAttribEnd, ptBuff->snoPower));
            if (nStackCount > 0 && (dwEndTick >= dwNow || bInfinite))
            {






                const LPCSTR CHARACTER_BUFF_TEMPLATE = "ConsoleCharacterDetails:ConsoleCharacterDetails_main.LayoutRoot.BuffTemplate";
                const UIControlSourceKey CHARACTER_BUFF_SOURCE_KEY = UIControlSourceKey::Parse(CHARACTER_BUFF_TEMPLATE, SNO_NONE, NULL, NULL);

                UIWindow * pControl = static_cast<UIWindow *>(UI_CreateControlFromTemplate(CHARACTER_BUFF_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("CharacterBuffEntry%d", snCount++), UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL), NULL));


                pControl->SetActive(TRUE);

                UIID idControl = pControl->GetID();

                tBuffPanel.AddChild(idControl);

                UIID idBuffImage = UIIDCreateFromParent(idControl, ".BuffImage");

                UIWindow & tWindow = UI_GetWindowFromID(idBuffImage);

                ImageHandle hImage = ptBuff->GetImage();
                if (hImage)
                {
                    ImageTextureFrame tImageTexFrame;
                    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tImageTexFrame);
                    tWindow.SetTextureFrame(tImageTexFrame);
                }

                UIID idBuffText = UIIDCreateFromParent(idControl, ".Description");
                UIText & tDescription = UI_GetTextFromID(idBuffText);

                CRefString text = UIBuffs::GetBuffTooltipText(*ptBuff);
                tDescription.SetText(text, FALSE);

                if (bIsBeneficial)
                {
                    UIID idBuffFrame = UIIDCreateFromParent(idControl, ".BuffFrame");
                    UIWindow & tWindowScoped = UI_GetWindowFromID(idBuffFrame);
                    tWindowScoped.SetActive(TRUE);
                }
                else
                {
                    UIID idDebuffFrame = UIIDCreateFromParent(idControl, ".DebuffFrame");
                    UIWindow & tWindowScoped = UI_GetWindowFromID(idDebuffFrame);
                    tWindowScoped.SetActive(TRUE);
                }

                BuffData tBuffData;
                tBuffData.idBuff = idControl;
                tBuffData.idBuffText = idBuffText;
                tBuffData.tBuff = *ptBuff;

                stBuffList.push_back(tBuffData);
            }
        }
    }
}

inline void GatherBuffs(const Player * ptPlayer, const UIBuffs::XPooledSortedBuffList & listBuffs, blz::vector<const UIBuffs::SortedBuff *, blz::allocator<const UIBuffs::SortedBuff *> > & vecOutput)
{



    POS pos = listBuffs.GetHeadPosition();
    while (pos)
    {
        const UIBuffs::SortedBuff & tBuff = listBuffs.GetNext(pos);
        if (tBuff.ptPlayer && ptPlayer->nPlayerIndex == tBuff.ptPlayer->nPlayerIndex)
        {
            vecOutput.push_back(&tBuff);
        }
    }
}



void DisplayBuffs(const Player * ptPlayer)
{

    UIBuffs::UIBuffGlobals * pBuffGlobals = UIBuffs::GetBuffGlobals();

    if (pBuffGlobals)
    {



        blz::vector<const UIBuffs::SortedBuff *, blz::allocator<const UIBuffs::SortedBuff *> > vecBeneficialBuffs, vecHarmfulBuffs;

        GatherBuffs(ptPlayer, pBuffGlobals->listBeneficialBuffs, vecBeneficialBuffs);
        GatherBuffs(ptPlayer, pBuffGlobals->listSkillButtonBuffs, vecBeneficialBuffs);
        GatherBuffs(ptPlayer, pBuffGlobals->listMinimapBuffs, vecBeneficialBuffs);
        GatherBuffs(ptPlayer, pBuffGlobals->listHarmfulBuffs, vecHarmfulBuffs);

        blz::stable_sort(blz::begin(vecBeneficialBuffs), blz::end(vecBeneficialBuffs),
            [](const UIBuffs::SortedBuff *& ptBuff1, const UIBuffs::SortedBuff *& ptBuff2) -> int
            {
                if (ptBuff1->snoPower == ptBuff2->snoPower)
                {
                    return ptBuff1->nBuffIdx - ptBuff2->nBuffIdx;
                }

                CRefString strName1 = UIBuffs::GetBuffName(ptBuff1->snoPower, ptBuff1->nBuffIdx);
                CRefString strName2 = UIBuffs::GetBuffName(ptBuff2->snoPower, ptBuff2->nBuffIdx);
                return strcasecmp(strName1.str(), strName2.str());
            });

        AddBuffsToDisplay(ptPlayer, vecBeneficialBuffs, true);
        AddBuffsToDisplay(ptPlayer, vecHarmfulBuffs, false);

        UIText & tNoBuffsText = UI_GetTextFromID(UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL_NO_BUFFS_TEXT);

        if (stBuffList.size() == 0)
        {
            tNoBuffsText.SetText(StringListFetch("ConsoleCharacterDetails:NoBuffsToDisplay").str(), FALSE);
            tNoBuffsText.SetActive(TRUE);
        }
        else
        {
            tNoBuffsText.SetActive(FALSE);
        }
    }
}


void UpdateBuffs(const Player * ptPlayer)
{
    size_t nBuffs = stBuffList.size();
    for (size_t n = 0; n < nBuffs; n++)
    {
        UIID idBuff = stBuffList[n].idBuff;
        UIText & tDescription = UI_GetTextFromID(stBuffList[n].idBuffText);
        CRefString text = UIBuffs::GetBuffTooltipText(stBuffList[n].tBuff);
        tDescription.SetText(text, FALSE);
    }
}







void SetReturnScreen(::Console::Screens::GameplayScreens ePrevScreen)
{
    sePrevScreen = ePrevScreen;
}



void OpenForController(int32 nControllerIndex)
{
    if (nControllerIndex != -1 && PlayerGetByControllerIndex(nControllerIndex))
    {
        snOpeningControllerIndex = nControllerIndex;
        sptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS, FALSE, GetUIIDNone());
        UI_SetControllerFocus(UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS);
        stStatsDataSource.PopulateInfoList(sptPlayer);
        DisplayBuffs(sptPlayer);
        UpdateButtonLegend();
        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS_LIST);
        snSelectedIndex = 1;
        tListBox.SetFirstVisibleItemIndex(0);
        tListBox.SetSelectedIndex(snSelectedIndex & 1 ? snSelectedIndex - 1 : -1);
        tListBox.RefreshVisibleItems();
        snLastUpdateTime = CGameGetPredictedServerTime();
    }
    GameSoftPause(TRUE);
}













void Close()
{
    GameSoftPause(FALSE);
    UIStackPanel & tBuffPanel = UI_GetStackPanelFromID(UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL);
    tBuffPanel.DeleteAllChildren();
    snCount = 0;



    stBuffList.clear();
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS_LIST);
    tListBox.ResetAllItems();

    stStatsDataSource.Clear();
    UI_SetControllerFocus(GetUIIDNone());
    ::Console::Screens::CloseModalDialog(UIWND_SKILLS_CONSOLE_CHARACTER_DETAILS);
}



void OnUpdate(const UIID &)
{
    const uint32 UPDATE_INTERVAL = 1;
    uint32 dwNow = CGameGetPredictedServerTime();
    if (dwNow - snLastUpdateTime > UPDATE_INTERVAL)
    {
        snLastUpdateTime = dwNow;
        UpdateBuffs(sptPlayer);
    }
    if (::Console::Screens::GetActionManager().AreActionsDirty())
    {
        UpdateButtonLegend();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }
}









void OnRightStick(const UIID &, const int32 nControllerIndex)
{
    const float SCROLL_SCALE = 4.0f;
    const float REBOUND_DEAD_ZONE = 0.2f;
    const float DEAD_ZONE = 0.2f;
    if (::Console::ControllerUtils::RStickIsInDeadZone(nControllerIndex, DEAD_ZONE, REBOUND_DEAD_ZONE))
    {
        return;
    }
    Vector2D vecScroll = ::Console::ControllerUtils::GetRStickPosition(nControllerIndex);
    vecScroll.y *= -SCROLL_SCALE;
    UIConsoleScrollBox & scrollbox = UI_GetConsoleScrollBoxFromID(UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL_SCROLL_BOX);
    float flLimit = scrollbox.GetScrollLimit();







    sflRightStickScrollAmount = Math::Pin(sflRightStickScrollAmount + vecScroll.y, 0.0f, flLimit);
    scrollbox.SetScrollPos(sflRightStickScrollAmount);
}





void OnButton2(const UIID &, const int32 nControllerIndex)
{
    ::Console::Screens::OpenToScreen(0, sePrevScreen, snOpeningControllerIndex, FALSE);
}







void OnL2(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer && !::Console::ControllerUtils::IsButtonDown(sptPlayer->nControllerIndex, 7))
    {
        UIUtils::Console::ScrollContentInScrollBox(UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_SCROLL_BOX, -4.0f);
    }
}









void OnR2(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer && !::Console::ControllerUtils::IsButtonDown(sptPlayer->nControllerIndex, 6))
    {
        UIUtils::Console::ScrollContentInScrollBox(UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_SCROLL_BOX, 4.0f);
    }
}










void UpdateButtonLegend()
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    if (UIUtils::Console::CanScrollContentInScrollBox(UIWND_SKILLS_CONSOLE_CHARACTER_DETAIL_SCROLL_BOX))
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
    }
    if (!(sptPlayer != NULL && InputIsSingleJoycon(sptPlayer->nControllerIndex)) && UIUtils::Console::CanScrollContentInScrollBox(UIWND_SKILLS_CONSOLE_CHARACTER_BUFF_PANEL_SCROLL_BOX))
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollBuffs");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK, ACTION_SCROLL_BUFFS_LIST, sScrollPrompt);
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    ::Console::Screens::UpdateButtonLegendText();
}







void HandleListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    stStatsDataSource.UpdateStatDetails(id, nOldVal, nNewVal);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

}  // namespace Console
}  // namespace UICharacterDetails
