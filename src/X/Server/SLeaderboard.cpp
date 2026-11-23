// Original file: X/Server/SLeaderboard.cpp
#include "../Common/Player.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/VisualInventory.h"
#include "../Common/ItemsSwig.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Protobuf/Leaderboard.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Client/Console/Online.h"
// DWARF: ActorInventory.cpp:5823.
void ACDInventorySavedStateStore(const ActorCommonData &, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> &, uint32, bool);
// DWARF: SLeaderboard.cpp:113.
namespace SLeaderboard { void FillInMemberMetadata(D3::Leaderboard::Member *, const Player *, bool); }
// DWARF: Online.cpp:4334 (signature also in owner CU).
namespace Console { namespace Online { BOOL PartyGetPartyMemberForPlayer(const Player *, PartyMember &); } }
// DWARF: ActorInventory.cpp:419.
InventorySlotType ActorInventoryTranslateFromSavedSlot(int32);










// The original inlines this file-local helper; its standalone body is not checkable.
static void ShrinkGeneratorForSnapshot(D3::Items::Generator * gen)
{
    // Inference: paired zero store and mask clear original durability alongside the other fields.
    gen->clear_max_durability();
    gen->clear_season_created();
    gen->clear_max_durability_original();
    gen->clear_trade_account_id();
    gen->clear_trade_ticks_left();
    gen->clear_hardcore();

    gen->clear_item_binding_level();
    gen->clear_item_unlock_timestamp();
}





































































void SLeaderboard::FillInMemberMetadata(D3::Leaderboard::Member * into, const Player * player, bool bIncludeHeroSnapshot)
{
    ActorCommonData & tACDPlayer = ACDGet(player->idPlayerACD);
    // Inference: preserve nonzero guild IDs; the other metadata fields are direct DWARF-member loads.
    into->set_account_id(player->tAccount.tHandle.m_id);
    into->set_hero_flags(player->tHeroStateData.uPlayerFlags);
    into->set_hero_gbid_class(tACDPlayer.hGB.gbid);
    into->set_hero_name(player->usHeroName);
    into->set_hero_season_created(player->uSeasonCreated);
    if (player->uGuildId) // Inference described above: nonzero guild IDs are retained.
        into->set_clan_id(player->uGuildId);
    into->set_hero_level(tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL));
    into->set_hero_alt_level(tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL));


    into->clear_account_id();
    into->clear_hero_name();

    Console::Online::PartyMember tPartyMember;
    if (Console::Online::PartyGetPartyMemberForPlayer(player, tPartyMember))
    {
        into->set_battle_tag(tPartyMember.szName);
        into->set_account_id(tPartyMember.idOnlineUser);
    }

    // Inference: the false branch skips snapshot construction and takes the common cleanup.
    if (!bIncludeHeroSnapshot)
        return;



    // Inference: DWARF records the initial backpack bit; the call passes all fourteen low bits.
    uint32 itemSlotFlags = 1;
    itemSlotFlags |= (1u << SLOT_PLAYER_BODYSLOT_END) - 1;





    D3::Hero::SavedDefinition savedHero;
    ACDInventorySavedStateStore(tACDPlayer,
        *savedHero.mutable_items()->mutable_items(), itemSlotFlags, false);



    // Inference: use saved player data; potion state starts false/zero as recorded by DWARF.
    const PlayerSavedData & playerSaveData = player->tHeroStateData.tSavedData;
    InventoryLocation potionLoc;
    bool hasPotion = false;
    int32 potionSlotIndex = 0;
    // Inference: only a valid potion ANN with a present inventory location contributes a slot.
    if (playerSaveData.tPotionBtn.m_annItem != ANN_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(playerSaveData.tPotionBtn.m_annItem));
        if (ptACDItem)
        {
            const InventoryLocation * ptInvLoc = ptACDItem->TryToGetInventoryLocation();
            if (ptInvLoc)
            {
                IVector2D tSlotSize;
                ACDInventorySlotGetSize(ptInvLoc->eSlot, tSlotSize);
                hasPotion = true;
                potionLoc = *ptInvLoc;
                // Inference: flatten the original y, width, x calculation into the slot index.
                potionSlotIndex = ptInvLoc->pt.y * tSlotSize.x + ptInvLoc->pt.x;
            }
        }
    }

    // Inference: ascending saved-item traversal and zeroed flags follow the original predicates.
    for (int i = 0; i < savedHero.items().items_size(); ++i)
    {
        const D3::Items::SavedItem & savedItem = savedHero.items().items(i);
        InventorySlotType itemSlotType = ActorInventoryTranslateFromSavedSlot(savedItem.item_slot());
        bool potion = false;


        bool add = false;
        // Inference: the original inclusive 1..14 range includes the buyback slot.
        add = itemSlotType >= SLOT_PLAYER_BODYSLOT_START && itemSlotType <= SLOT_PLAYER_BODYSLOT_END;



        // Inference: compute the square match only after potion presence and slot equality.
        if (hasPotion && itemSlotType == potionLoc.eSlot)
        {
            potion = savedItem.square_index() == potionSlotIndex;
            add = add || potion;

        }



        if (add)
        {
            D3::Leaderboard::HeroEquippedItem * equippedItem = into->mutable_hero_snapshot()->add_equipped_items();

            *equippedItem->mutable_generator() = savedHero.items().items(i).generator();

            if (potion)
            {
                equippedItem->set_is_potion(true);
            }


            VisualInventorySlot visSlot = VisualInventorySlotFromInventorySlot(ActorInventoryTranslateFromSavedSlot(savedHero.items().items(i).item_slot()));

            if (visSlot != VIS_INVALID)
            {
                equippedItem->set_visual_inventory_slot(visSlot);
                equippedItem->set_item_visual_gbid(tACDPlayer.ptVisualInventory->tEquipmentCurrent.tVisualItem[visSlot].gbidItem);
                equippedItem->set_item_effect_type(tACDPlayer.ptVisualInventory->tEquipmentCurrent.tVisualItem[visSlot].eItemEffectType);
                equippedItem->set_effect_level(tACDPlayer.ptVisualInventory->tEquipmentCurrent.tVisualItem[visSlot].nEffectLevel);

                if (tACDPlayer.ptVisualInventory->tEquipmentCurrent.tVisualItem[visSlot].eDyeType)
                {
                    equippedItem->set_dye_type(tACDPlayer.ptVisualInventory->tEquipmentCurrent.tVisualItem[visSlot].eDyeType);
                }
            }


            ShrinkGeneratorForSnapshot(equippedItem->mutable_generator());

            for (int j = 0; j < equippedItem->mutable_generator()->contents_size(); ++j)
            {


                ShrinkGeneratorForSnapshot(equippedItem->mutable_generator()->mutable_contents(j)->mutable_generator());
            }
        }
    }

    // Inference: both original loops visit every fixed skill/trait slot, including empty slots.
    for (int i = 0; i < NUM_SKILL_SLOTS; ++i)
    {
        const ActiveSkillSavedData & activeSkillData = playerSaveData.tActiveSkills[i];
        D3::Profile::SkillWithRune * skill = into->mutable_hero_snapshot()->mutable_active_skills()->add_runes();
        skill->set_skill(activeSkillData.snoSkill);
        skill->set_rune_type(activeSkillData.eRuneType);
    }


    for (int i = 0; i < NUM_TRAIT_SLOTS; ++i)
    {
        into->mutable_hero_snapshot()->mutable_active_traits()->add_sno_traits(
            playerSaveData.snoTraits[i]);
    }

    // Inference: the original stores a zero high word and the local hero identifier low word.
    into->mutable_hero_snapshot()->mutable_hero_id()->set_id_high(0);
    into->mutable_hero_snapshot()->mutable_hero_id()->set_id_low(player->uHeroId);



    GBHandleList listBonuses(GBGetHandlePool());
    GBEnumerate(GB_PARAGON_BONUSES, listBonuses);

    POS pos = listBonuses.GetHeadPosition();
    while (pos)
    {
        const GBHandle & hGBBonus = listBonuses.GetNext(pos);
        // Inference: omit zero allocations; the original branch skips their protobuf records.
        int32 points = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hGBBonus.gbid));
        if (points)
        {
            D3::Profile::ParagonPointAllocation * data = into->mutable_hero_snapshot()->mutable_paragon_allocation()->add_category();
            data->set_slot(hGBBonus.gbid);
            data->set_points(points);
        }
    }

    // Inference: the four cosmetic slots are traversed in ascending order, skipping invalid IDs.
    for (int i = 0; i < VCIS_NUM_SLOTS; ++i)
    {
        GBID gbid = tACDPlayer.ptVisualInventory->tEquipmentCurrent.tCosmeticItems[i].gbidItem;

        if (gbid != GBID_NONE)
        {
            D3::Leaderboard::HeroCosmeticItem * cosmeticItem = into->mutable_hero_snapshot()->add_cosmetic_items();
            cosmeticItem->set_gbid(gbid);
            cosmeticItem->set_cosmetic_visual_inventory_slot(i);
        }
    }

    // Inference: preserve three slots, substituting the invalid sentinel for nonexistent items.
    for (int32 i = 0; i < 3; ++i)
    {
        GBID gbidItem = playerSaveData.gbidLegendaryPowers[i];
        if (gbidItem != GBID_NONE &&
            GBIDExists(gbidItem, GB_ITEMS))
        {
            into->mutable_hero_snapshot()->add_gbid_legendary_powers(gbidItem);
        }
        else
        {
            into->mutable_hero_snapshot()->add_gbid_legendary_powers(GBID_NONE);
        }
    }
}



#include "SGame.h"
#include "SGameManager.h"
#include "PlayerList.h"
#include "../Common/NephalemRiftCommon.h"
#include "../Common/Leaderboard.h"
#include "../Common/OnlineService/OnlineService.h"
#include "../Common/Networking/GameMessage.h"
#include "../Sigma/BlzExt/fixed_vector.h"
// DWARF: SLeaderboard.cpp:344.
namespace SLeaderboard { void PostRiftTime(int, int, int, const Player *, bool); }
// DWARF: Player.cpp:2049.
Player * PlayerGetNextInTieredRift(const Player *);
// DWARF: Leaderboard.cpp:37.
uint64 LeaderboardMakeRiftScore(uint32, int32, uint32);
// DWARF: Leaderboard.cpp:786.
blz::string LeaderboardCompressMessage(const google::protobuf::Message &, int);













void SLeaderboard::PostRiftTime(
    int riftLevel,
    int millisecondsElapsed,
    int playerCount,
    const Player * firstPlayerInRift,
    bool cheated)
{




    // Inference: reject nonpositive rift levels or team sizes before querying the level cap.
    if (riftLevel <= 0 || playerCount <= 0)
        return;





    // Inference: the cap comparison takes the common early exit.
    if (riftLevel > NephalemRift::GetMaxTieredRiftLevel())
        return;







    Blizzard::Time::Timestamp ts = Blizzard::Time::GetTimestamp();

    ts = Console::Online::GetNetworkTimestamp();



    // Inference: the signed division/multiply at original line 383 uses 10000000.
    const Blizzard::Time::Timestamp CENTISECONDS_IN_NANOSECOND = 10000000;


    ts = (ts / CENTISECONDS_IN_NANOSECOND) * CENTISECONDS_IN_NANOSECOND;

    D3::Leaderboard::PostScore post;
    // Inference: absence of the first player exits after constructing post.
    if (!firstPlayerInRift)
        return;


    // Inference: these booleans use the original weekly-challenge result and seasonal/hardcore bits.
    const bool challengeGame = GameIsWeeklyChallenge();
    const bool seasonalGame = (SGameManager::GameGet().tStartParams.dwCreationFlags & (1u << GAMECREATEPARAM_SEASON)) != 0;
    const bool hardcoreHero = (firstPlayerInRift->tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE)) != 0;

    // Inference: normal runs require an active season; challenge runs bypass this rejection.
    if (!challengeGame && (!seasonalGame || OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE))
        return;





    // Inference: the normal-run switch maps team size; solo maps the original seven class-table entries.
    if (!challengeGame)
    {
        switch (playerCount) {
        case 1: {
            const ActorCommonData & tACDPlayer = ACDGet(firstPlayerInRift->idPlayerACD);

            switch (PlayerClassFromGBID(tACDPlayer.hGB.gbid))
            {
            case PLAYER_CLASS_BARBARIAN:
                post.set_category_id(LB_RIFT_SOLO_BARBARIAN);
                break;
            case PLAYER_CLASS_CRUSADER:
                post.set_category_id(LB_RIFT_SOLO_CRUSADER);
                break;
            case PLAYER_CLASS_DEMONHUNTER:
                post.set_category_id(LB_RIFT_SOLO_DEMONHUNTER);
                break;
            case PLAYER_CLASS_MONK:
                post.set_category_id(LB_RIFT_SOLO_MONK);
                break;
            case PLAYER_CLASS_WITCHDOCTOR:
                post.set_category_id(LB_RIFT_SOLO_WITCHDOCTOR);
                break;
            case PLAYER_CLASS_WIZARD:
                post.set_category_id(LB_RIFT_SOLO_WIZARD);
                break;
            case PLAYER_CLASS_NECROMANCER:
                post.set_category_id(LB_RIFT_SOLO_NECROMANCER);
                break;
            default:
                return;
            }
            break;
        }
        case 2:
            post.set_category_id(LB_RIFT_TEAM_2);
            break;
        case 3:
            post.set_category_id(LB_RIFT_TEAM_3);
            break;
        case 4:
            post.set_category_id(LB_RIFT_TEAM_4);
            break;
        default:
            return;
        }
    }









    // Inference: only a true cheated flag materializes the original cheated metadata record.
    if (cheated)
    {
        post.mutable_metadata()->set_cheated(true);
    }
    // Inference: challenge runs require server globals and use the recorded challenge partition.
    if (challengeGame)
    {
        post.set_scope_id(LP_CHALLENGE);
        if (!SGameManager::GameGet().ptSGameGlobals)
            return;



        uint32 uPartition = SGameManager::GameGet().ptSGameGlobals->tChallengeRift.nChallengeNumber;










        post.set_partition_id(uPartition);

        switch (playerCount)
        {
        case 1:
            post.set_category_id(LB_CHALLENGE_RIFT_TEAM_1);
            break;
        case 2:
            post.set_category_id(LB_CHALLENGE_RIFT_TEAM_2);
            break;
        case 3:
            post.set_category_id(LB_CHALLENGE_RIFT_TEAM_3);
            break;
        case 4:
            post.set_category_id(LB_CHALLENGE_RIFT_TEAM_4);
            break;
        }
    }
    else
    {


        // Inference: retain the four explicit seasonal/hardcore combinations and corresponding partitions.
        if (seasonalGame && hardcoreHero)
        {
            post.set_scope_id(LP_SEASONAL_HARDCORE);
            post.set_partition_id(OnlineService::GetSeasonNum());
        }
        else if (!seasonalGame && !hardcoreHero)
        {
            post.set_scope_id(LP_SOFTCORE);
            post.set_partition_id(OnlineService::GetLeaderboardEra());
        }
        else if (seasonalGame && !hardcoreHero)
        {
            post.set_scope_id(LP_SEASONAL_SOFTCORE);
            post.set_partition_id(OnlineService::GetSeasonNum());
        }
        else if (!seasonalGame && hardcoreHero)
        {
            post.set_scope_id(LP_HARDCORE);
            post.set_partition_id(OnlineService::GetLeaderboardEra());
        }
    }
    post.set_score(LeaderboardMakeRiftScore(riftLevel, millisecondsElapsed, Blizzard::Time::ToUnixTime(ts)));
    post.set_timestamp(ts);

    D3::Leaderboard::Metadata team;


    team.set_level_seed(SGameManager::GameGet().ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed);


    blzext::fixed_vector<const Player *, 4> tTeamMemberToPlayer;

    // Inference: enumerate the original linked team order, retaining each player for its private message.
    for (const Player * teamIter = firstPlayerInRift; teamIter; teamIter = PlayerGetNextInTieredRift(teamIter))
    {
        FillInMemberMetadata(team.add_team_member(), teamIter, true);

        tTeamMemberToPlayer.push_back(teamIter);
    }

    // Inference: each recipient moves to index zero; other members lose snapshot/visual payloads.
    for (int i = 0; i < team.team_member_size(); ++i)
    {
        post.set_game_account_id(team.team_member(i).account_id());

        D3::Leaderboard::Metadata * md = post.mutable_metadata();
        md->CopyFrom(team);
        md->mutable_team_member()->SwapElements(i, 0);


        for (int t = 1; t < md->team_member_size(); ++t)
        {
            md->mutable_team_member(t)->clear_hero_visual_equipment();
            md->mutable_team_member(t)->clear_hero_snapshot();
        }












        // Inference: original compression level is 9, followed by the score-message type 526.
        D3::Leaderboard::CompressedPostScore tCompressed;
        tCompressed.set_data(LeaderboardCompressMessage(post, 9));

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, *tTeamMemberToPlayer[i]);

        MessageSendToClient(listDestPlayers, SMSG_RIFT_POST_SCORE, tCompressed);

    }
}

