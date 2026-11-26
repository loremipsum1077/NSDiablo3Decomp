#include "ActorCommonData.h"
#include "../Client/Gfx/GfxVB.h"  // Per-unit vertex normals initialized by the original startup routine.
#include "Attributes/FastAttribConstants.h"
#include "Attributes/FastAttribute.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "ActorInventory.h"
#include "Items.h"
#include "Definitions/GameBalanceDefinition.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Server/PlayerList.h"
#include "../Server/Powers/PowerAPI.h"

struct Unk4b1d80
{
    blz::vector<blz::string> unk_00;
    int32 unk_18;
    int32 unk_1c;
    blz::string unk_20;
};

struct Unk4b1f10
{
    blz::vector<blz::pair<CurrencyType, int32>> unk_00;
    blz::vector<Unk4b1d80> unk_18;
};

static_assert(sizeof(Unk4b1d80) == 0x48, "item requirement");
static_assert(sizeof(Unk4b1f10) == 0x30, "sacrifice recipe");

// Recipe data from the global initializers at 0x71004b4c50 and 0x71004b21c0.
static blz::vector<blz::string> unk_71019010f0 = {
    "Unique_Helm_009_x1", "P69_Unique_Helm_Set_06", "Unique_Helm_Set_14_x1",
    "Unique_Helm_Set_07_x1", "Unique_Helm_Set_03_p2", "Unique_Helm_Set_08_x1",
    "Unique_Helm_Set_11_x1", "Unique_Helm_Set_01_p3", "P67_Unique_Helm_Set_02",
    "Unique_SpiritStone_009_x1", "P69_Necro_Set_5_Helm", "P6_Necro_Set_1_Helm",
    "P6_Necro_Set_2_Helm", "P6_Necro_Set_3_Helm", "P6_Necro_Set_4_Helm",
    "Unique_Helm_Set_06_x1", "Unique_Helm_Set_13_x1", "Unique_Helm_Set_02_p2",
    "P68_Unique_Helm_Set_03", "Unique_Helm_010_x1", "Unique_Helm_008_x1",
    "Unique_Helm_Set_05_x1", "Unique_Helm_Set_15_x1", "Unique_Helm_Set_01_p2",
    "P68_Unique_Helm_Set_05", "Unique_Helm_Set_10_x1", "Unique_Helm_Set_12_x1",
    "Unique_Helm_Set_01_p1", "Unique_Helm_Set_03_p3", "P67_Unique_Helm_Set_01",
    "Unique_VoodooMask_007_x1", "Unique_Helm_Set_09_x1", "Unique_Helm_Set_16_x1",
    "Unique_Helm_Set_02_p3", "P68_Unique_Helm_Set_04"
};
static blz::vector<blz::string> unk_7101901108 = {
    "x1_Diamond_02", "x1_Diamond_03", "x1_Diamond_04",
    "x1_Diamond_05", "x1_Diamond_06", "x1_Diamond_07",
    "x1_Diamond_08", "x1_Diamond_09", "x1_Diamond_10"
};
static blz::vector<Unk4b1f10> unk_7101901180 = {
    {{{21, 55}}, {}},
    {{{21, 110}}, {}},
    {{{21, 165}}, {}},
    {{}, {{{}, 0, 0, "P75_DarkAlchemyMajorNodeDisplay_004"}}}
};
static blz::vector<Unk4b1f10> unk_7101901198 = {
    {{{CURRENCY_REUSABLE_PARTS, 10}}, {}},
    {{{CURRENCY_ARCANE_DUST, 15}, {CURRENCY_REUSABLE_PARTS, 20}}, {{unk_7101901108, 1, 0, "P75_DarkAlchemyMinorNodeDisplay_002"}}},
    {{{20, 1}, {CURRENCY_DEATHS_BREATH, 10}}, {}},
    {{}, {{unk_71019010f0, 1, 0, "P75_DarkAlchemyMinorNodeDisplay_004"}}},
    {{{CURRENCY_FORGOTTEN_SOUL, 20}, {CURRENCY_KHANDURAN_RUNE, 10}, {CURRENCY_CALDEUM_NIGHTSHADE, 10}, {CURRENCY_ARREAT_WAR_TAPESTRY, 10}, {CURRENCY_CORRUPTED_ANGEL_FLESH, 10}, {CURRENCY_WESTMARCH_HOLY_WATER, 10}}, {}},
    {{{CURRENCY_DEMON_ORGAN_DIABLO, 1}, {CURRENCY_DEMON_ORGAN_GHOM, 1}, {CURRENCY_DEMON_ORGAN_SIEGE_BREAKER, 1}, {CURRENCY_DEMON_ORGAN_SKELETON_KING, 1}}, {}},
    {{}, {{{"Unique_Bracer_103_x1"}, 1, 0, ""}}},
    {{{CURRENCY_FORGOTTEN_SOUL, 30}}, {}},
    {{{CURRENCY_BLOODSHARDS, 1100}}, {}},
    {{{CURRENCY_DEATHS_BREATH, 20}}, {{{"x1_Ruby_10"}, 1, 0, ""}, {{"unique_ring_107_x1"}, 1, 0, ""}}},
    {{{CURRENCY_KHANDURAN_RUNE, 30}, {CURRENCY_CALDEUM_NIGHTSHADE, 30}, {CURRENCY_ARREAT_WAR_TAPESTRY, 30}, {CURRENCY_CORRUPTED_ANGEL_FLESH, 30}, {CURRENCY_WESTMARCH_HOLY_WATER, 30}}, {{{"x1_Emerald_10"}, 1, 0, ""}}},
    {{{20, 20}}, {{{"Consumable_Add_Sockets_1"}, 1, 1, ""}}},
    {{{CURRENCY_BLOODSHARDS, 1300}}, {}},
    {{}, {{{"SwarmRiftKey"}, 1, 0, ""}}},
    {{}, {{{"ChallengeRiftRewardBag"}, 1, 0, ""}}},
    {{{CURRENCY_FORGOTTEN_SOUL, 250}}, {}},
    {{{CURRENCY_BLOODSHARDS, 1400}}, {}},
    {{}, {{{"x1_Amulet_norm_unique_25"}, 1, 2, "P75_DarkAlchemyMinorNodeDisplay_018"}}},
    {{}, {{{"class_clues1"}, 1, 0, ""}, {{"class_clues2"}, 1, 0, ""}, {{"class_clues3"}, 1, 0, ""}, {{"class_clues4"}, 1, 0, ""}}},
    {{{CURRENCY_KHANDURAN_RUNE, 50}, {CURRENCY_CALDEUM_NIGHTSHADE, 50}, {CURRENCY_ARREAT_WAR_TAPESTRY, 50}, {CURRENCY_CORRUPTED_ANGEL_FLESH, 50}, {CURRENCY_WESTMARCH_HOLY_WATER, 50}}, {{{"Unique_Ring_004_x1"}, 1, 2, "P75_DarkAlchemyMinorNodeDisplay_020"}}},
    {{{CURRENCY_FORGOTTEN_SOUL, 300}, {CURRENCY_DEATHS_BREATH, 500}}, {}},
    {{{CURRENCY_BLOODSHARDS, 1500}}, {}},
    {{}, {{{"swarm_rift_gem_125plus"}, 1, 0, "P75_DarkAlchemyMinorNodeDisplay_023"}}},
    {{}, {{{"augmented_item"}, 1, 0, "P75_DarkAlchemyMinorNodeDisplay_024"}}},
    {{}, {{{"StaffOfCow"}, 1, 0, ""}}},
    {{{CURRENCY_BLOODSHARDS, 1600}}, {}}
};
static blz::vector<blz::string> unk_71019011b0 = {"swarm_rift_gem_125plus", "augmented_item", "class_clues1", "class_clues2", "class_clues3", "class_clues4"};
static blz::vector<blz::string> unk_71019011c8 = {"Unique_Bracer_103_x1", "unique_ring_107_x1", "augmented_item", "Unique_Ring_004_x1", "x1_Amulet_norm_unique_25"};
static blz::unordered_map<const char *, GBID> unk_71019011f8;
static SNO unk_7101901218[30];
static SNO unk_71010abbd8 = INVALID_SNO;
static const char *unk_71010abad8[4] = {"class_clues1", "class_clues2", "class_clues3", "class_clues4"};
static const char *unk_71010abaf8[7][4] = {
    {"p4_SetDung_Clue_Barb_Kings", "p4_SetDung_Clue_Barb_Might", "p4_SetDung_Clue_Barb_Raekor", "p4_SetDung_Clue_Barb_Wastes"},
    {"p4_SetDung_Clue_Cru_Akkhan", "p4_SetDung_Clue_Cru_Roland", "p4_SetDung_Clue_Cru_Seeker", "p4_SetDung_Clue_Cru_Thorns"},
    {"p4_SetDung_Clue_DH_Ess", "p4_SetDung_Clue_DH_Mar", "p4_SetDung_Clue_DH_Nat", "p4_SetDung_Clue_DH_Shadows"},
    {"p4_SetDung_Clue_Monk_Innas", "p4_SetDung_Clue_Monk_Storms", "p4_SetDung_Clue_Monk_Sunwuko", "p4_SetDung_Clue_Monk_Uliana"},
    {"p4_SetDung_Clue_WD_Haunt", "p4_SetDung_Clue_WD_Jade", "p4_SetDung_Clue_WD_Spider", "p4_SetDung_Clue_WD_Tooth"},
    {"p4_SetDung_Clue_Wiz_Firebird", "p4_SetDung_Clue_Wiz_Opus", "p4_SetDung_Clue_Wiz_Rasha", "p4_SetDung_Clue_Wiz_Vyr"},
    {"p6_SetDung_Clue_Necro_Blood", "p6_SetDung_Clue_Necro_Bone", "p6_SetDung_Clue_Necro_Plague", "p6_SetDung_Clue_Necro_Saint"}
};

BOOL FindGBHandleByActor(SNO snoActor, GBHandle & handle);
WeaponClass GetWeaponClass(GBID left, GBID right);

extern "C" void sub_71004b6e40(const Unk4b1f10 & recipe);

// Probably: cache the powers and resolve the item names used by the altar recipes.
// NON_MATCHING: The 28 class-clue lookups compile from the recovered name table. The startup body is four bytes shorter.
extern "C" void sub_71004b6800()
{
    for (uint32 i = 0; i < 30; ++i)
        unk_7101901218[i] = GlobalSNOGet(GLOBALSNO_UNK_1599 + i);
    for (const auto & name : unk_71019010f0)
        unk_71019011c8.push_back(name);
    for (const auto & recipe : unk_7101901198)
        sub_71004b6e40(recipe);
    for (const auto & recipe : unk_7101901180)
        sub_71004b6e40(recipe);
    for (const auto & row : unk_71010abaf8)
        for (const char * const & name : row)
            unk_71019011f8[name] = GBIDExists(name, GB_ITEMS);
    unk_71010abbd8 = SNOFileLookupGUID(ACTOR_SNO, "p73_SwarmRift_Gem_01");
}

// Probably: resolve the display item and the named alternatives of one sacrifice recipe.
extern "C" void sub_71004b6e40(const Unk4b1f10 & recipe)
{
    for (const auto & item : recipe.unk_18)
    {
        auto resolve = [](const blz::string & name) {
            GBID gbid = GBIDExists(name.c_str(), GB_ITEMS);
            if (gbid == INVALID_GBID || gbid == GBID_NONE)
            {
                SNO actor = SNOFileLookupGUID(ACTOR_SNO, name.c_str());
                GBHandle handle;
                if (actor != INVALID_SNO && FindGBHandleByActor(actor, handle))
                    gbid = handle.gbid;
            }
            unk_71019011f8[name.c_str()] = gbid;
        };
        if (!item.unk_20.empty())
            resolve(item.unk_20);
        for (const auto & name : item.unk_00)
        {
            if (unk_71019011f8.find(name.c_str()) == unk_71019011f8.end() &&
                blz::find(unk_71019011b0.begin(), unk_71019011b0.end(), name) == unk_71019011b0.end())
                resolve(name);
        }
    }
}

// Probably: Dark Alchemy app shutdown callback (a single RET in 2.7.7).
extern "C" void sub_71004b71a0()
{
}

// Probably: dark alchemy "get unlocked node powers": appends the power SNO of every unlocked node (bits 0..29 of
// ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) to arPowers. Caller: ActorCommonData.cpp.
// NON_MATCHING: The unlock mask stays in a register across vector growth. The target spills and reloads it.
extern "C" void sub_71004b71b0(const ActorCommonData & tACD, blz::vector<SNO> & arPowers)
{
    uint32 uNodes = tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES);
    if (uNodes == 0)
        return;
    for (int32 i = 0; i < 30; i++)
    {
        if (uNodes & (1u << i))
            arPowers.push_back(unk_7101901218[i]);
    }
}


extern "C" BOOL sub_71004b72d0(const ActorCommonData & tACD)
{
    return (tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 0x1c000000) != 0;
}

extern "C" uint32 sub_71004b72f0(const ActorCommonData & tACD)
{
    return tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES);
}

extern "C" BOOL sub_71004b7300(const ActorCommonData & tACD)
{
    uint32 nodes = sub_71004b72f0(tACD);
    size_t count = 0;
    while (nodes)
    {
        count += nodes & 1;
        nodes >>= 1;
    }
    return static_cast<uint32>(count) == 30;
}

struct Unk4b92f0
{
    uint64 unk_00;
    GBID unk_08;
    int32 unk_0c;
};

extern "C" void sub_71004b92f0(ACDID, InventorySlotType, void *);
extern "C" void sub_71004b93e0(ACDID, InventorySlotType, void *);
extern "C" void sub_71004b94c0(ACDID, InventorySlotType, void *);
extern "C" BOOL sub_71004b81e0(const ActorCommonData &, uint32);
extern "C" int32 sub_71004b8330(const ActorCommonData &);

// Probably: calculate the currency and item costs for the next unlock in a category.
// NON_MATCHING: The inventory loops, class-clue lookups and vector growth use a different register and branch layout.
extern "C" BOOL sub_71004b7340(const ActorCommonData & actor, int32 category, Unk1784d0 & costs)
{
    const Unk4b1f10 * recipe = NULL;
    if (category == 1)
    {
        uint32 rank = sub_71004b8330(actor);
        if (rank == 26)
            return FALSE;
        recipe = &unk_7101901198[rank];
    }
    else if (category == 2)
    {
        uint32 rank = sub_71004b81e0(actor, 26) + sub_71004b81e0(actor, 27) + sub_71004b81e0(actor, 28);
        if (rank == 3)
            rank += sub_71004b81e0(actor, 29);
        if (rank == 4)
            return FALSE;
        recipe = &unk_7101901180[rank];
    }
    if (!recipe)
        return FALSE;

    for (const auto & currency : recipe->unk_00)
    {
        auto & requirement = costs.unk_00.push_back();
        auto & entry = requirement.unk_00.push_back();
        entry.unk_00 = 0;
        entry.unk_04 = currency.first;
        entry.unk_38 = actor.GetCurrencyAmount(currency.first);
        requirement.unk_18 = currency.second;
        requirement.unk_20 += entry.unk_38;
    }

    for (const auto & item : recipe->unk_18)
    {
        bool selectItem = false;
        for (const auto & name : item.unk_00)
        {
            if (blz::find(unk_71019011c8.begin(), unk_71019011c8.end(), name) != unk_71019011c8.end() ||
                (blz::find(unk_71010abad8, unk_71010abad8 + 4, name) == unk_71010abad8 + 4 &&
                 unk_71019011f8.find(name.c_str()) == unk_71019011f8.end()))
            {
                selectItem = true;
                break;
            }
        }

        auto & requirement = (selectItem ? costs.unk_18 : costs.unk_00).push_back();
        requirement.unk_18 = item.unk_18;
        requirement.unk_2c = item.unk_1c;
        if (!item.unk_20.empty())
            requirement.unk_28 = unk_71019011f8.find(item.unk_20.c_str())->second;
        for (const auto & name : item.unk_00)
        {
            // At 0x71004b8080 the target calls blz::vector<Unk4b8080>::push_back().
            auto & entry = requirement.unk_00.push_back();
            auto it = unk_71019011f8.find(name.c_str());
            if (it == unk_71019011f8.end())
            {
                auto clue = blz::find(unk_71010abad8, unk_71010abad8 + 4, name);
                if (clue != unk_71010abad8 + 4)
                {
                    // The class jump table at 0x7100ebda9c selects these four-name rows.
                    const uint32 rows[] = {2, 0, 5, 4, 3, 1, 6};
                    uint32 playerClass = PlayerGetPlayerClass(&actor);
                    const char * key = playerClass < 7 ? unk_71010abaf8[rows[playerClass]][clue - unk_71010abad8] : NULL;
                    it = unk_71019011f8.find(key);
                }
            }
            entry.unk_08 = it == unk_71019011f8.end() ? INVALID_GBID : it->second;
            entry.unk_00 = entry.unk_08 == INVALID_GBID ? 2 : 1;
            entry.unk_10 = name;
            entry.unk_38 = 0;
            Unk4b92f0 data = {0, entry.unk_08, 0};
            InventorySearchCallback callback = NULL;
            if (entry.unk_08 != INVALID_GBID)
            {
                data.unk_0c = requirement.unk_2c;
                callback = sub_71004b92f0;
            }
            else if (name == "augmented_item")
                callback = sub_71004b93e0;
            else if (name == "swarm_rift_gem_125plus")
                callback = sub_71004b94c0;
            if (callback)
            {
                ACDInventoryIterateItems(actor, SLOT_PLAYER_BACKPACK, 0, callback, &data);
                if (!selectItem)
                    ACDInventoryIterateItems(actor, SLOT_PLAYER_SHARED_STASH, 0, callback, &data);
                entry.unk_38 = data.unk_00;
            }
            requirement.unk_20 += entry.unk_38;
        }
    }
    return TRUE;
}

extern "C" BOOL sub_71004b81d0(uint32 index)
{
    return index < 30;
}

extern "C" BOOL sub_71004b81e0(const ActorCommonData & tACD, uint32 index)
{
    if (!sub_71004b81d0(index))
        return FALSE;
    return (sub_71004b72f0(tACD) & (1u << index)) != 0;
}

// Prerequisite lists constructed at 0x71004b5dec..0x71004b6328.
static blz::vector<blz::vector<uint32>> unk_71019011e0 = {
    {}, {0}, {0}, {1}, {1}, {2}, {2}, {3, 4, 5, 6},
    {14}, {14}, {15}, {15}, {16}, {16}, {7}, {7}, {7},
    {23}, {23}, {24}, {24}, {25}, {25},
    {8, 9, 24}, {10, 11, 23, 25}, {12, 13, 24},
    {17, 18}, {19, 20}, {21, 22},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
     15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28}
};

// Probably: test whether the node is locked and its prerequisites have been met.
// NON_MATCHING: The two prerequisite scans allocate and advance their iterators differently.
extern "C" BOOL sub_71004b8230(const ActorCommonData & tACD, uint32 index)
{
    if (!sub_71004b81d0(index))
        return FALSE;
    uint32 nodes = sub_71004b72f0(tACD);
    if (nodes & (1u << index))
        return FALSE;
    const auto & prerequisites = unk_71019011e0[index];
    if (prerequisites.empty())
        return TRUE;
    if (index == 29)
    {
        for (uint32 node : prerequisites)
            if (!(nodes & (1u << node)))
                return FALSE;
        return TRUE;
    }
    for (uint32 node : prerequisites)
        if (nodes & (1u << node))
            return TRUE;
    return FALSE;
}

static inline int32 sDarkAlchemySealCount(const ActorCommonData & tACD)
{
    uint32 nodes = sub_71004b72f0(tACD);
    size_t count = 0;
    while (nodes)
    {
        count += nodes & 1;
        nodes >>= 1;
    }

    int32 potions = sub_71004b81e0(tACD, 26) + sub_71004b81e0(tACD, 27) + sub_71004b81e0(tACD, 28);
    if (potions == 3)
        potions += sub_71004b81e0(tACD, 29);
    return count - potions;
}

extern "C" int32 sub_71004b8330(const ActorCommonData & tACD)
{
    return sDarkAlchemySealCount(tACD);
}

extern "C" int32 sub_71004b83e0(uint32 index)
{
    if (index - 26u > 3u)
        return sub_71004b81d0(index);
    return 2;
}

// Probably: identify which category of Dark Alchemy unlock remains available.
extern "C" int32 sub_71004b8400(const ActorCommonData & tACD)
{
    if (static_cast<uint32>(sDarkAlchemySealCount(tACD)) < 26)
        return 1;
    int32 potions = sub_71004b81e0(tACD, 26) + sub_71004b81e0(tACD, 27) + sub_71004b81e0(tACD, 28);
    if (potions == 3)
        potions += sub_71004b81e0(tACD, 29);
    return static_cast<uint32>(potions) < 4 ? 2 : 0;
}

extern "C" BOOL sub_71004b86e0(const ActorCommonData &, uint32, const ActorCommonData &);

// Probably: validate the sacrifice and report whether an inventory item must be selected.
// NON_MATCHING: The cost vectors occupy different stack slots and clang schedules the two capacity checks differently.
extern "C" GameError sub_71004b8530(const ActorCommonData & actor, uint32 node, const ActorCommonData * item)
{
    Unk1784d0 costs;
    if (!sub_71004b7340(actor, sub_71004b83e0(node), costs))
        return GAMEERROR_INSUFFICIENT_FUNDS;
    for (const auto & requirement : costs.unk_00)
        if (requirement.unk_20 < requirement.unk_18)
            return GAMEERROR_INSUFFICIENT_FUNDS;
    for (const auto & requirement : costs.unk_18)
        if (requirement.unk_20 < requirement.unk_18)
            return GAMEERROR_INSUFFICIENT_FUNDS;
    if (!item)
        return costs.unk_18.empty() ? GAMEERROR_NONE : GAMEERROR_UNK_52;
    if (costs.unk_18.empty() || !item->IsItem() || ItemIsUnidentified(*item))
        return GAMEERROR_UNK_52;
    const auto & location = item->GetLocation().tInvLocation;
    InventorySlotType slot = location.eSlot;
    if (!ACDInventoryIsSlotPlayer(slot) || slot == SLOT_PLAYER_BUYBACK ||
        slot == SLOT_PLAYER_BASE_HEALTHPOTION || ACDInventoryGetItemInSlot(location) != item->id ||
        location.idACDContainer != actor.id || (!ACDIsPlayer(location.idACDContainer) && !actor.GetPetOwnerPlayer()))
        return GAMEERROR_UNK_52;
    return sub_71004b86e0(actor, node, *item) ? GAMEERROR_NONE : GAMEERROR_UNK_52;
}

// Probably: test the selected item against every manual sacrifice requirement.
// NON_MATCHING: The item-kind tests and loop exit for a selected alternative have a different branch layout.
extern "C" BOOL sub_71004b86e0(const ActorCommonData & actor, uint32 node, const ActorCommonData & item)
{
    Unk1784d0 costs;
    sub_71004b7340(actor, sub_71004b83e0(node), costs);
    for (const auto & requirement : costs.unk_18)
    {
        const Unk4b8080 * match = NULL;
        for (const auto & alternative : requirement.unk_00)
        {
            if (match)
                break;
            if (alternative.unk_00 == 1 && item.GetGBHandle().gbid == alternative.unk_08)
            {
                if (requirement.unk_2c != 2 || ItemIsAncientRank(item))
                    match = &alternative;
            }
            else if (alternative.unk_00 == 2)
            {
                if (alternative.unk_10 == "augmented_item")
                {
                    if (item.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK) > 0 &&
                        static_cast<uint32>(GetWeaponClass(item.GetGBHandle().gbid, GBID_NONE)) < NUM_WEAPON_CLASSES)
                        match = &alternative;
                }
                else if (alternative.unk_10 == "swarm_rift_gem_125plus" &&
                         item.snoActor == unk_71010abbd8 && item.AttributesGetInt(ATTRIBUTE_JEWEL_RANK) > 124)
                    match = &alternative;
            }
        }
        if (!match)
            return FALSE;
    }
    return TRUE;
}

// The four helpers below test the node's power as a buff on the actor first (PowerAPI_BuffFindOnACD with node
// power table entries 2, 0, 0 and 1), then the node bit in ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES.

// Probably: dark alchemy node 2 "no level requirement": returns 1 instead of nLevel when node 2 is active.
// Callers: ItemOwnerMeetsRequirements (Items.cpp), ActorInventory.cpp, UIHotbarButtonControl.cpp.
extern "C" int32 sub_71004b8900(const ActorCommonData * ptACD, int32 nLevel)
{
    if (ptACD == NULL)
        return nLevel;
    if (PowerAPI_BuffFindOnACD(unk_7101901218[2], -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return 1;
    return (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 4) ? 1 : nLevel;
}

// Probably: dark alchemy node 0 doubling of an integer (Player.cpp: streak timer tolerance in ticks).
// NON_MATCHING: The buff lookup and bit-zero doubling test use a different return path.
extern "C" int32 sub_71004b8980(const ActorCommonData * ptACDPlayer, int32 nValue)
{
    if (ptACDPlayer == NULL)
        return nValue;
    if (PowerAPI_BuffFindOnACD(unk_7101901218[0], -1, *ptACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return nValue << 1;
    return nValue << (ptACDPlayer->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 1);
}

// Probably: dark alchemy node 0 doubling of a float (SPlayer.cpp: massacre XP multiplier).
// NON_MATCHING: The floating-point doubling and fallback returns use a different branch layout.
extern "C" float sub_71004b89f0(const ActorCommonData * ptACDPlayer, float flValue)
{
    if (ptACDPlayer == NULL)
        return flValue;
    if (PowerAPI_BuffFindOnACD(unk_7101901218[0], -1, *ptACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return flValue + flValue;
    return (ptACDPlayer->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 1) ? flValue + flValue : flValue;
}

// Probably: dark alchemy node 1 active on the actor (Experience.cpp: rest XP refill).
extern "C" BOOL sub_71004b8ae0(const ActorCommonData * ptACD)
{
    if (ptACD == NULL)
        return FALSE;
    if (PowerAPI_BuffFindOnACD(unk_7101901218[1], -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return TRUE;
    return (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) >> 1) & 1;
}

// Probably: get the power SNO for a Dark Alchemy node.
// NON_MATCHING: The power lookup addresses the array directly. The target uses the merged unit-data base.
extern "C" SNO sub_71004b8b50(uint32 index)
{
    return sub_71004b81d0(index) ? unk_7101901218[index] : INVALID_SNO;
}

extern "C" BOOL sub_71004b8b80(const ActorCommonData & first, const ActorCommonData & second)
{
    if (ACDGetLevelArea(second) != ACDGetLevelArea(first))
        return FALSE;
    WorldPlace firstPlace = first.GetPlace();
    WorldPlace secondPlace = second.GetPlace();
    if (firstPlace.idSWorld != secondPlace.idSWorld)
        return FALSE;
    float dx = secondPlace.wp.x - firstPlace.wp.x;
    float dy = secondPlace.wp.y - firstPlace.wp.y;
    return !(dx * dx + dy * dy > 225.0f);
}

// Probably: append a node's prerequisites to the caller's list.
extern "C" void sub_71004b8c30(uint32 index, blz::vector<uint32> & result)
{
    for (uint32 node : unk_71019011e0[index])
        result.push_back(node);
}

// Probably: dark alchemy node 1 active on tACD or on any player local to it (BrainComponents.cpp: TRUE keeps rest XP
// on death).
// NON_MATCHING: The local-player scan and buff lookup use a different spill and branch layout.
extern "C" BOOL sub_71004b8d40(const ActorCommonData & tACD)
{
    if (PowerAPI_BuffFindOnACD(unk_7101901218[1], -1, tACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return TRUE;
    if (tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 2)
        return TRUE;

    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer == NULL)
        return FALSE;

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllPlayersLocalToPlayer(listPlayers, ptPlayer);
    POS pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptLocalPlayer = listPlayers.GetNext(pos);
        const ActorCommonData * ptACD = ACDTryToGet(ptLocalPlayer->idPlayerACD);
        if (ptACD == NULL)
            continue;
        if (PowerAPI_BuffFindOnACD(unk_7101901218[1], -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
            return TRUE;
        if (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 2)
            return TRUE;
    }
    return FALSE;
}

// Probably: count matching identified items, applying the ancient requirement when requested.
extern "C" void sub_71004b92f0(ACDID id, InventorySlotType, void * userData)
{
    auto & data = *static_cast<Unk4b92f0 *>(userData);
    const ActorCommonData * item = ACDTryToGet(id);
    if (!item || ItemIsUnidentified(*item) || item->GetGBHandle().gbid != data.unk_08)
        return;
    if (data.unk_0c == 2 && !ItemIsAncientRank(*item))
        return;
    uint64 count = item->GetStackCount();
    data.unk_00 += count ? count : 1;
}

// Probably: count identified, augmented weapons for the altar.
extern "C" void sub_71004b93e0(ACDID id, InventorySlotType, void * userData)
{
    auto & data = *static_cast<Unk4b92f0 *>(userData);
    const ActorCommonData * item = ACDTryToGet(id);
    if (!item || ItemIsUnidentified(*item))
        return;
    if (static_cast<uint32>(GetWeaponClass(item->GetGBHandle().gbid, GBID_NONE)) < NUM_WEAPON_CLASSES)
        data.unk_00 += item->AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK) > 0;
}

// Probably: count identified Whispers of Atonement of rank 125 or higher.
extern "C" void sub_71004b94c0(ACDID id, InventorySlotType, void * userData)
{
    auto & data = *static_cast<Unk4b92f0 *>(userData);
    const ActorCommonData * item = ACDTryToGet(id);
    if (item && item->snoActor == unk_71010abbd8 && !ItemIsUnidentified(*item))
        data.unk_00 += item->AttributesGetInt(ATTRIBUTE_JEWEL_RANK) > 124;
}
