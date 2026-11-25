#include "../../Gfx/GfxVB.h"
#include "../UI.h"
#include "../UIIDs.h"
#include "../Controls/UITextControl.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../../Contrib/Contrib/blz/include/blz/map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"

struct unk_191aa0
{
    blz::string unk_0;
    blz::string unk_28[7];
    int32 unk_140[7];
    int32 unk_15c = 0;
};
static_assert(sizeof(unk_191aa0) == 0x160, "leaderboard category size");

// 0x71017e5bf0. The initializer at 0x7100192390 populates this map.
static blz::map<int32, unk_191aa0> unk_17e5bf0;
extern "C" void sub_7100192390();

extern "C" BOOL sub_7100192320(int32 category)
{
    return (uint32)category > 2;
}

extern "C" int32 sub_7100191ef0(int32 index, int32 category, BOOL hardcore, BOOL solo)
{
    return ((uint32)index << 9) | ((uint32)category << 25) | ((hardcore != FALSE) << 1) | (solo != FALSE) | 0x20000;
}

// NON_MATCHING: Clang folds the 42 cases into a value table. The target uses a jump table.
extern "C" int32 sub_7100191e50(int64 id)
{
    switch ((int32)id)
    {
    case 41:
    case 47:
    case 53:
    case 59:
    case 65:
    case 71:
    case 77:
        return 1;
    case 42:
    case 48:
    case 54:
    case 60:
    case 66:
    case 72:
    case 78:
        return 2;
    case 43:
    case 49:
    case 55:
    case 61:
    case 67:
    case 73:
    case 79:
        return 3;
    case 44:
    case 50:
    case 56:
    case 62:
    case 68:
    case 74:
    case 80:
        return 4;
    case 45:
    case 51:
    case 57:
    case 63:
    case 69:
    case 75:
    case 81:
        return 5;
    case 46:
    case 52:
    case 58:
    case 64:
    case 70:
    case 76:
    case 82:
        return 6;
    default:
        return 0;
    }
}

extern "C" int32 sub_7100191eb0(int32 category)
{
    if (unk_17e5bf0.empty())
        sub_7100192390();
    return unk_17e5bf0[category].unk_15c;
}

extern "C" void sub_7100192290(int32 category)
{
    if (unk_17e5bf0.empty())
        sub_7100192390();
    unk_17e5bf0[category].unk_15c = unk_17e5bf0[category].unk_15c + 1;
    if (unk_17e5bf0[category].unk_15c >= 7)
        unk_17e5bf0[category].unk_15c = 0;
}

extern "C" void sub_71001921f0(int32 category)
{
    if (unk_17e5bf0.empty())
        sub_7100192390();
    unk_17e5bf0[category].unk_15c = unk_17e5bf0[category].unk_15c - 1;
    if (unk_17e5bf0[category].unk_15c < 0)
        unk_17e5bf0[category].unk_15c = 6;
}

// NON_MATCHING: The index branches and stack/register allocation differ (284 versus 280 bytes).
extern "C" BOOL sub_7100191c80(LeaderboardKeyAndPartition &key, int32 index, uint32 category,
                                BOOL hardcore, BOOL solo, BOOL previous)
{
    key.m_partition = OnlineService::GetSeasonNum() - (previous != FALSE);
    key.m_scope = !hardcore ? (solo ? (LeaderboardScope)9 : LP_SEASONAL_SOFTCORE) :
                              (solo ? (LeaderboardScope)8 : LP_SEASONAL_HARDCORE);
    int32 id = LB_NONE;
    switch (index)
    {
    case 0: id = LB_RIFT_TEAM_2; break;
    case 1: id = LB_RIFT_TEAM_3; break;
    case 2: id = LB_RIFT_TEAM_4; break;
    case 3: case 4: case 5: case 6: case 7: case 8: case 9:
    {
        if (unk_17e5bf0.empty())
            sub_7100192390();
        unk_191aa0 entry = unk_17e5bf0[index];
        id = entry.unk_140[category];
        break;
    }
    }
    key.m_id = id;
    return id != LB_NONE;
}

// NON_MATCHING: The map lookup and index load are scheduled in a different order.
extern "C" BOOL sub_71001918d0(LeaderboardKeyAndPartition &key, int32 index, BOOL hardcore,
                                BOOL solo, BOOL previous)
{
    if (unk_17e5bf0.empty())
        sub_7100192390();
    unk_191aa0 entry = unk_17e5bf0[index];
    return sub_7100191c80(key, index, entry.unk_15c, hardcore, solo, previous);
}

// NON_MATCHING: The category range test and stack slots differ in the filter label update.
extern "C" void sub_7100191f20(int32 category)
{
    blz::string title;
    if (category >= 3 && category <= 9)
    {
        if (unk_17e5bf0.empty())
            sub_7100192390();
        uint32 filter = unk_17e5bf0[category].unk_15c;
        blz::string name = unk_17e5bf0[category].unk_28[filter];
        title.format("%s", name.c_str());
    }
    else if ((uint32)category <= 2)
        title = StringListFetch(FormatRefString("ConsoleLeaderboards:GreaterRiftsLeaderboard%d", category + 2)).str();
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_TITLE).SetText(title.c_str(), FALSE);
}

extern "C" void sub_7100192390()
{
    unk_191aa0 barbarian;
    barbarian.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_BARBARIAN, TRUE).str();
    barbarian.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    barbarian.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianNoSet").str();
    barbarian.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianSet1").str();
    barbarian.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianSet2").str();
    barbarian.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianSet3").str();
    barbarian.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianSet4").str();
    barbarian.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloBarbarianSet5").str();
    barbarian.unk_140[0] = LB_RIFT_SOLO_BARBARIAN;
    barbarian.unk_140[6] = 46;
    barbarian.unk_140[1] = 41;
    barbarian.unk_140[2] = 42;
    barbarian.unk_140[3] = 43;
    barbarian.unk_140[4] = 44;
    barbarian.unk_140[5] = 45;

    unk_191aa0 crusader;
    crusader.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_CRUSADER, TRUE).str();
    crusader.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    crusader.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderNoSet").str();
    crusader.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderSet1").str();
    crusader.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderSet2").str();
    crusader.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderSet3").str();
    crusader.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderSet4").str();
    crusader.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloCrusaderSet5").str();
    crusader.unk_140[0] = LB_RIFT_SOLO_CRUSADER;
    crusader.unk_140[6] = 52;
    crusader.unk_140[1] = 47;
    crusader.unk_140[2] = 48;
    crusader.unk_140[3] = 49;
    crusader.unk_140[4] = 50;
    crusader.unk_140[5] = 51;

    unk_191aa0 demonhunter;
    demonhunter.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_DEMONHUNTER, TRUE).str();
    demonhunter.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    demonhunter.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterNoSet").str();
    demonhunter.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterSet1").str();
    demonhunter.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterSet2").str();
    demonhunter.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterSet3").str();
    demonhunter.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterSet4").str();
    demonhunter.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloDemonHunterSet5").str();
    demonhunter.unk_140[0] = LB_RIFT_SOLO_DEMONHUNTER;
    demonhunter.unk_140[6] = 58;
    demonhunter.unk_140[1] = 53;
    demonhunter.unk_140[2] = 54;
    demonhunter.unk_140[3] = 55;
    demonhunter.unk_140[4] = 56;
    demonhunter.unk_140[5] = 57;

    unk_191aa0 monk;
    monk.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_MONK, TRUE).str();
    monk.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    monk.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkNoSet").str();
    monk.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkSet1").str();
    monk.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkSet2").str();
    monk.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkSet3").str();
    monk.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkSet4").str();
    monk.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloMonkSet5").str();
    monk.unk_140[0] = LB_RIFT_SOLO_MONK;
    monk.unk_140[6] = 64;
    monk.unk_140[1] = 59;
    monk.unk_140[2] = 60;
    monk.unk_140[3] = 61;
    monk.unk_140[4] = 62;
    monk.unk_140[5] = 63;

    unk_191aa0 necromancer;
    necromancer.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_NECROMANCER, TRUE).str();
    necromancer.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    necromancer.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerNoSet").str();
    necromancer.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerSet1").str();
    necromancer.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerSet2").str();
    necromancer.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerSet3").str();
    necromancer.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerSet4").str();
    necromancer.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloNecromancerSet5").str();
    necromancer.unk_140[0] = LB_RIFT_SOLO_NECROMANCER;
    necromancer.unk_140[6] = 82;
    necromancer.unk_140[1] = 77;
    necromancer.unk_140[2] = 78;
    necromancer.unk_140[3] = 79;
    necromancer.unk_140[4] = 80;
    necromancer.unk_140[5] = 81;

    unk_191aa0 witchdoctor;
    witchdoctor.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_WITCHDOCTOR, TRUE).str();
    witchdoctor.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    witchdoctor.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorNoSet").str();
    witchdoctor.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorSet1").str();
    witchdoctor.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorSet2").str();
    witchdoctor.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorSet3").str();
    witchdoctor.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorSet4").str();
    witchdoctor.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWitchDoctorSet5").str();
    witchdoctor.unk_140[0] = LB_RIFT_SOLO_WITCHDOCTOR;
    witchdoctor.unk_140[6] = 70;
    witchdoctor.unk_140[1] = 65;
    witchdoctor.unk_140[2] = 66;
    witchdoctor.unk_140[3] = 67;
    witchdoctor.unk_140[4] = 68;
    witchdoctor.unk_140[5] = 69;

    unk_191aa0 wizard;
    wizard.unk_0 = PlayerGetClassUIName(PLAYER_CLASS_WIZARD, TRUE).str();
    wizard.unk_28[0] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloOverall").str();
    wizard.unk_28[6] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardNoSet").str();
    wizard.unk_28[1] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardSet1").str();
    wizard.unk_28[2] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardSet2").str();
    wizard.unk_28[3] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardSet3").str();
    wizard.unk_28[4] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardSet4").str();
    wizard.unk_28[5] = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_LEADERBOARDS), "FilterSoloWizardSet5").str();
    wizard.unk_140[0] = LB_RIFT_SOLO_WIZARD;
    wizard.unk_140[6] = 76;
    wizard.unk_140[1] = 71;
    wizard.unk_140[2] = 72;
    wizard.unk_140[3] = 73;
    wizard.unk_140[4] = 74;
    wizard.unk_140[5] = 75;

    unk_17e5bf0.insert(blz::make_pair(3, barbarian));
    unk_17e5bf0.insert(blz::make_pair(4, crusader));
    unk_17e5bf0.insert(blz::make_pair(5, demonhunter));
    unk_17e5bf0.insert(blz::make_pair(6, monk));
    unk_17e5bf0.insert(blz::make_pair(7, necromancer));
    unk_17e5bf0.insert(blz::make_pair(8, witchdoctor));
    unk_17e5bf0.insert(blz::make_pair(9, wizard));
}
