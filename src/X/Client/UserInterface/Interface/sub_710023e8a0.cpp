#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/Powers/PowersStruct.h"
#include "../../../Common/TagMaps/PowerTags.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/ActorCommonData.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../Controls/UIWindowControl.h"
#include "../../CPlayer.h"
#include "../UIID.h"
#include "../../Gfx/GfxVB.h"  // Per-unit vertex normals initialized by the original startup routine.
#include "../UI.h"

void CACDAttributeSetValue(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tValue, const BOOL fBypassRActorChanges);  // CActorCommonData.cpp:58
CRefString PowerTrace(SNO snoPower);
CRefString StringListEntryReplace(LPCSTR szString, LPCSTR szToken, LPCSTR szReplacement, BOOL bReplaceAll);  // StringListDefinition.cpp
namespace UISkills
{
BOOL ResolvePowerTag(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &,
                     float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);
}

extern "C" SNO sub_71004b8b50(uint32 node);
extern "C" uint32 sub_71004b72f0(const ActorCommonData & actor);
extern "C" void sub_71003d0b70(ACDID id, SNO power);
static uint32 unk_71017ff43c;

// Screen order from the initializer at 0x710023f304.
static blz::vector<uint32> unk_71017ff458 = {
    0, 1, 2, 3, 4, 5, 6, 7, 14, 15, 16, 8, 9, 10, 11,
    12, 13, 23, 24, 25, 17, 18, 19, 20, 21, 22, 26, 27, 28, 29
};
static UIID unk_71017ff4b0[30];

struct Unk23e590
{
    UIID unk_00, unk_08, unk_10;
};
static blz::unordered_map<uint64, Unk23e590> unk_71017ff490;
extern UIID unk_7101862568;

// Probably: find or cache the three controls for a line between two altar nodes.
// NON_MATCHING: The 39 accepted line names are scanned in a loop. The target uses two chains of literal comparisons.
extern "C" BOOL sub_710023dcb0(uint32 a, uint32 b, UIID & line, UIID & on, UIID & enabled)
{
    uint64 key = (uint64(a) << 32) | b;
    auto it = unk_71017ff490.find(key);
    if (it == unk_71017ff490.end())
    {
        static const char * const names[] = {
            "line_1_2", "line_1_3", "line_2_4", "line_2_5", "line_3_6",
            "line_3_7", "line_4_8", "line_5_8", "line_6_8", "line_7_8",
            "line_8_9", "line_8_10", "line_8_11", "line_9_12", "line_9_13",
            "line_12_18", "line_13_18", "line_18_21", "line_18_22", "line_21_27",
            "line_22_27", "line_10_14", "line_10_15", "line_14_19", "line_15_19",
            "line_19_23", "line_19_24", "line_23_28", "line_24_28", "line_11_16",
            "line_11_17", "line_16_20", "line_17_20", "line_20_25", "line_20_26",
            "line_25_29", "line_26_29", "line_18_19", "line_19_20"
        };
        CRefString label;
        label.Format("line_%d_%d", a, b);
        auto valid = [&]() {
            for (const char * name : names)
                if (label == name)
                    return true;
            return false;
        };
        if (!valid())
        {
            label.Format("line_%d_%d", b, a);
            if (!valid())
                return FALSE;
        }
        CRefString base, lit, active;
        base.Format(".Lines.%s", label.str());
        lit.Format(".Lines.%s.%s_on", label.str(), label.str());
        active.Format(".Lines.%s.%s_enabled", label.str(), label.str());
        UIID root = UIIDCreateFromParent(unk_7101862568, ".LayoutRoot");
        Unk23e590 ids = {UIIDCreateFromParent(root, base.str()), UIIDCreateFromParent(root, lit.str()),
                         UIIDCreateFromParent(root, active.str())};
        it = unk_71017ff490.insert(blz::make_pair(key, ids)).first;
        unk_71017ff490.insert(blz::make_pair((uint64(b) << 32) | a, it->second));
    }
    line = it->second.unk_00;
    on = it->second.unk_08;
    enabled = it->second.unk_10;
    return TRUE;
}

// Probably: cache the glow control for each altar node in screen order.
extern "C" void sub_710023e7f0()
{
    extern UIID unk_7101862570;
    for (uint32 i = 0; i < 30; ++i)
        unk_71017ff4b0[i] = UIIDCreateFromParent(unk_7101862570, CRefString().Format(".Node_%d.UnlockFX_%d", i, i));
}

// Probably: get the glow control ID for a node in screen order.
// NON_MATCHING: The table is addressed directly. The target adds the table offset to a merged data base.
extern "C" UIID & sub_710023e8a0(uint32 index)
{
    return unk_71017ff4b0[index];
}

// Probably: format a node power description with one extra skill rank and a minimum player level of one.
// NON_MATCHING: Damage formatting and saved attribute values use different stack slots and register lifetimes.
extern "C" CRefString sub_710023e8c0(ActorCommonData & tACDOwner, SNO snoPower)
{
    CRefString skill = PowerTrace(snoPower);
    CRefString label;
    Prefs::GetGameplayPrefs().ShowAdvancedTooltips();
    label = CRefString().Format("%s_desc", skill.str());
    CRefString usTip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), label);
    AttribGroupID idFastAttrib = tACDOwner.AttributesGetAttributeGroup();

    int32 nOldLevel = FastAttribGetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower));
    int32 nNewLevel = nOldLevel + 1;
    FastAttribValue nNewValue = nNewLevel;
    CACDAttributeSetValue(tACDOwner, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower), nNewValue, TRUE);

    int32 nOldPlayerLevel = tACDOwner.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    int32 nAdjustedPlayerLevel = Math::Max(1, nOldPlayerLevel);
    CACDAttributeSetValue(tACDOwner, FastAttribKey(ATTRIBUTE_LEVEL), nAdjustedPlayerLevel, TRUE);

    usTip = UIToolTips_ProcessTip(&tACDOwner, idFastAttrib, -1, snoPower, nNewLevel, usTip, TTVC_NORMAL_COLORING, UISkills::ResolvePowerTag, NULL, NULL);

    PowerDamageData tDamageData;
    float flDamageMin = 0.0f;
    float flDamageMax = 0.0f;

    SNO alternate;
    {
        ScopedPowerDefinitionLockR power(snoPower);
        alternate = PowerGeneralTagGetSNO(power, TAG_POWER_DAMAGE_DISPLAY_POWER);
    }
    if (alternate == SNO_NONE)
        alternate = snoPower;
    PowerGetDamageData(AutoPowerDefinitionR(alternate), ACDTryToGet(tACDOwner.id), POWERID_INVALID, tDamageData, NULL, FALSE);
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

    return LanguageRules::ApplyLanguageRules(usTip.str());
}

// Probably: map a screen position to its unlock bit index.
extern "C" uint32 sub_710023edc0(uint32 index)
{
    return unk_71017ff458[index];
}

// Probably: find the position of an unlock bit in screen order.
// NON_MATCHING: Pointer subtraction lowers to an arithmetic shift. The target uses a logical shift for this nonnegative distance.
extern "C" size_t sub_710023edd0(uint32 node)
{
    return blz::find(unk_71017ff458.begin(), unk_71017ff458.end(), node) - unk_71017ff458.begin();
}

// Probably: notify the player and replay the glow animation for each newly unlocked node.
// NON_MATCHING: Clang lays out the two unlock-mask tests and the glow-control address differently.
extern "C" void sub_710023ee30(BOOL notify)
{
    ACDID id = LocalPlayerACDGet();
    const ActorCommonData * actor = ACDTryToGet(id);
    uint32 nodes = actor ? sub_71004b72f0(*actor) : 0;
    if (notify && nodes != unk_71017ff43c)
    {
        uint32 previous = unk_71017ff43c;
        for (uint32 node = 0; node < 32; ++node)
        {
            uint32 bit = 1u << node;
            if ((nodes & bit) && !(previous & bit))
            {
                sub_71003d0b70(id, sub_71004b8b50(node));
                UIID glow = sub_710023e8a0(sub_710023edd0(node));
                UI_GetWindowFromID(glow).SetActive(FALSE);
                UI_GetWindowFromID(glow).SetActive(TRUE);
            }
        }
    }
    unk_71017ff43c = nodes;
}
