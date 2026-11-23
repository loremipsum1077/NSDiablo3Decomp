// Original file: X/Server/OnlineService/AccountMigration.cpp

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/AccountMigration.pb.h"
#include "../../Common/Protobuf/CS.pb.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../Common/Protobuf/AttributeSerializer.pb.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Items.h"
#include "../../Common/Protobuf/Profile.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Protobuf/ItemCrafting.pb.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"

#include "../../Common/OnlineService/OnlineService.h"
int64 CurrencyDataGetCount(const CurrencyType eCurrencyType, const D3::Items::CurrencySavedData & data);  // X/Common/Items.cpp
void CurrencyDataSetCount(const CurrencyType eCurrencyType, D3::Items::CurrencySavedData & data, int64 count);  // X/Common/Items.cpp
void HealTransmogs(const D3::Account::SavedDefinition & tSrc, D3::Account::SavedDefinition & tDst, const OnlineService::GameAccountHandle & handle, const char * szLogHeader);  // X/Server/OnlineService/DataHealUtils.cpp
int32 AccountDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, int32 nDefault);  // X/Common/AccountDefinition.cpp

#include "../../Common/Crafting/ItemCrafting.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_set.h"
// DWARF AccountMigration.h:68, exact 0x20 layout; constructor is unused here.
namespace OnlineService { namespace AccountMigration {
struct SeasonMigrationData {
    SeasonMigrationData(D3::Account::AccountPartition *, D3::Account::AccountPartition *, const char *);
    D3::Account::AccountPartition * m_ptNonSeasonPartition;
    D3::Account::AccountPartition * m_ptSeasonPartition;
    const char * m_szLogHeader;
    bool m_republishPresence;
};



} }
// DWARF X/Common/Definitions/AccountDefinition.cpp declarations (GetUInt64:401, WriteUInt64:416).
uint64 AccountDefinitionSavedAttributeGetUInt64(const D3::AttributeSerializer::SavedAttributes &, const FastAttribKey &, const FastAttribKey &);
void AccountDefinitionSavedAttributeWriteUInt64(D3::AttributeSerializer::SavedAttributes &, const FastAttribKey &, const FastAttribKey &, uint64);
void AccountDefinitionSavedAttributeWriteInt(D3::AttributeSerializer::SavedAttributes &, const FastAttribKey &, int32);
void AccountDefinitionSavedAttributeRemoveInt(D3::AttributeSerializer::SavedAttributes &, const FastAttribKey &);
// DWARF Player.h:663 enumerators.

enum DataPersistFlags { DPF_DIRTY_PERSIST=0, DPF_DIRTY_CLIENTSYNC=1, DPF_PERSIST_IN_PROGRESS=2, DPF_PERSISTED=3 };
#include "../../Common/Game.h"
#include "../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../Common/Achievements/AchievementsEngine/achievements_types.h"
#include "../../Common/Protobuf/Achievements.pb.h"
#include "../../Common/Partition.h"
#include "../../Common/Definitions/HeroDefinition.h"
#include "../../Common/Quests/QuestsSwig.h"
// DWARF X/Common/Quests/Quests.cpp:1622.
BOOL QuestStepIsReachableByStep(SNO snoQuest, int32 nStepUIDSource, int32 nStepUIDDest);
void AccountDefinition_ToDefaults(D3::Account::SavedDefinition & tDefinition);  // DWARF X/Common/Definitions/AccountDefinition.cpp:317






















namespace OnlineService {
namespace AccountMigration {
static void _AccountMigrate_109_110_UpdateAccountAttributes(const D3::AccountMigration::ReadData_109 & tReadData, D3::AccountMigration::WriteData_110 & tWriteData);
static void _AccountMigrate_111_112_MigrateBlacksmithLevels(D3::CS::Snapshot & tSnapshot);
static void _AccountMigrate_115_116_CheckSoloGriftCompletion(D3::CS::Snapshot & tSnapshot);
static void _AccountMigrate_110_111_MigrateGold(D3::CS::Snapshot & tSnapshot);
static void _AccountMigrate_108_109_UpdatePartitions(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData);
static void _AccountMigrate_108_109_CreateMinis(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData);
static void _AccountMigrate_107_108_CreatePartitions(const D3::AccountMigration::ReadData_107 & tReadData, D3::AccountMigration::WriteData_108 & tWriteData);
static void _AccountMigrate_107_108_ConvertParagonLevels(const D3::AccountMigration::ReadData_107 & tReadData, D3::AccountMigration::WriteData_108 & tWriteData);
static void _AccountMigrate_107_108_ConvertHeroes(const D3::AccountMigration::ReadData_107 &, D3::AccountMigration::WriteData_108 &);
}
}




namespace OnlineService {
namespace AccountMigration {
static void _AccountMigrate_114_115_HealAccountWideTransmogs(const D3::AccountMigration::ReadData_114 &, D3::AccountMigration::WriteData_115 &);
// LINKER-REMOVED: concrete DIE low_pc 0, 0xfbc bytes; body from its DWARF inline tree and line table (row 107:6 only before the helper calls; early return FALSE is inferred).
BOOL Migrate_107_108(
    const D3::AccountMigration::ReadData_107 & tReadData,
    D3::AccountMigration::WriteData_108 & tWriteData)
{
	if (tReadData.account_data().version() != 107)
	{
		// Inference: only one row (107:6) before the helper calls; early-return form, no own row for return FALSE.
		return FALSE;
	}
    _AccountMigrate_107_108_CreatePartitions(tReadData, tWriteData);
    _AccountMigrate_107_108_ConvertParagonLevels(tReadData, tWriteData);
    _AccountMigrate_107_108_ConvertHeroes(tReadData, tWriteData);
    // Inference: no row of its own; returns TRUE like the other completed migrations.
    return TRUE;
}



BOOL Migrate_ConsoleVanilla_108(
    const D3::AccountMigration::ReadData_107 & tReadData,
    D3::AccountMigration::WriteData_108 & tWriteData)
{
    _AccountMigrate_107_108_ConvertHeroes(tReadData, tWriteData);
    // Inference: original wrapper returns TRUE unconditionally after converting heroes.
    return TRUE;
}



BOOL Migrate_108_109(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData)


{
    if (tReadData.account_data().version() == 108)
    {


        tWriteData.mutable_account_data()->set_version(109);
        _AccountMigrate_108_109_UpdatePartitions(tReadData, tWriteData);


        _AccountMigrate_108_109_CreateMinis(tReadData, tWriteData);
        return TRUE;
    }
    return FALSE;
}


BOOL Migrate_109_110(const D3::AccountMigration::ReadData_109 & tReadData, D3::AccountMigration::WriteData_110 & tWriteData)
{
    if (tReadData.account_data().version() != 109)
    {
        return FALSE;
    }

    tWriteData.mutable_account_data()->set_version(110);

    _AccountMigrate_109_110_UpdateAccountAttributes(tReadData, tWriteData);

    return TRUE;
}




// NON_MATCHING: 1028 vs 1016; differs only inside the inlined blz::vector push_back growth path (MigrateGold 727).
BOOL Migrate_110_111(
    D3::CS::Snapshot & tSnapshot)
{
    if (tSnapshot.definition().version() == 110)
    {



        tSnapshot.mutable_definition()->set_version(111);
        tSnapshot.mutable_definition()->mutable_digest()->set_version(111);

        _AccountMigrate_110_111_MigrateGold(tSnapshot);
        return TRUE;
    }

    return FALSE;
}



BOOL Migrate_111_112(D3::CS::Snapshot & tSnapshot)
{
    if (tSnapshot.definition().version() == 111)
    {
        tSnapshot.mutable_definition()->set_version(112);
        tSnapshot.mutable_definition()->mutable_digest()->set_version(112);

        _AccountMigrate_111_112_MigrateBlacksmithLevels(tSnapshot);
        return TRUE;
    }
    return FALSE;
}





BOOL Migrate_112_113(D3::CS::Snapshot & tSnapshot)

{

    if (tSnapshot.definition().version() == 112)
    {
        tSnapshot.mutable_definition()->set_version(113);
        tSnapshot.mutable_definition()->mutable_digest()->set_version(113);
        return TRUE;
    }






    return FALSE;
}
























BOOL Migrate_114_115(
    const D3::AccountMigration::ReadData_114 & tReadData,
    D3::AccountMigration::WriteData_115 & tWriteData)
{
    if (tReadData.account_data().snapshot().definition().version() != 114)
    {
        return FALSE;
    }

    _AccountMigrate_114_115_HealAccountWideTransmogs(tReadData, tWriteData);


    tWriteData.mutable_account_data()->mutable_snapshot()->mutable_definition()->set_version(115);
    tWriteData.mutable_account_data()->mutable_snapshot()->mutable_definition()->mutable_digest()->set_version(115);
    // Inference: the successful path returns the original constant TRUE.
    return TRUE;
}



BOOL Migrate_115_116(D3::CS::Snapshot & tSnapshot)
{
    if (tSnapshot.definition().version() == 115)
    {
        _AccountMigrate_115_116_CheckSoloGriftCompletion(tSnapshot);

        tSnapshot.mutable_definition()->set_version(116);
        tSnapshot.mutable_definition()->mutable_digest()->set_version(116);
        return TRUE;
    }

    return FALSE;
}




// DWARF decl 286, DW_INL_inlined into Migrate_107_108 only.
static void _AccountMigrate_107_108_CreatePartitions(
	const D3::AccountMigration::ReadData_107 & tReadData,
	D3::AccountMigration::WriteData_108 & tWriteData)
{
	tWriteData.mutable_account_data()->set_version(108);
	tWriteData.mutable_account_data()->set_flags(tReadData.account_data().flags());


	D3::Account::AccountPartition * ptNormal = tWriteData.mutable_account_data()->add_partitions();
	ptNormal->set_partition_id(ACCOUNT_PARTITION_NORMAL);
	ptNormal->set_accepted_license_bits(tReadData.account_data().deprecated_accepted_license_bits());
	*ptNormal->mutable_saved_attributes() = tReadData.account_data().saved_attributes();
	*ptNormal->mutable_crafter_data() = tReadData.account_data().deprecated_crafter_normal_data();
	ptNormal->mutable_gold_id_deprecated()->set_id_high(0);
	ptNormal->mutable_gold_id_deprecated()->set_id_low(tReadData.account_data().deprecated_gold_id_normal().id_low());
	*ptNormal->mutable_items() = D3::Items::ItemList::default_instance();
	// Inference 301/311/325/336: right-hand side is an out-of-line call at the column after "= " (no inlined accessor); ItemList::default_instance assumed.

	D3::Account::AccountPartition * ptHardcore = tWriteData.mutable_account_data()->add_partitions();
	ptHardcore->set_partition_id(ACCOUNT_PARTITION_HARDCORE);
	ptHardcore->set_accepted_license_bits(0);
	*ptHardcore->mutable_saved_attributes() = tReadData.account_data().saved_attributes_hardcore();
	*ptHardcore->mutable_crafter_data() = tReadData.account_data().deprecated_crafter_hardcore_data();
	ptHardcore->mutable_gold_id_deprecated()->set_id_high(0);
	ptHardcore->mutable_gold_id_deprecated()->set_id_low(tReadData.account_data().deprecated_gold_id_hardcore().id_low());
	*ptHardcore->mutable_items() = D3::Items::ItemList::default_instance();
	// Inference 314: one out-of-line call at column 2 between tDefault construction and the season partitions.
	D3::Account::SavedDefinition tDefault;
	AccountDefinition_ToDefaults(tDefault);


	D3::Account::AccountPartition * ptSeasonNormal = tWriteData.mutable_account_data()->add_partitions();
	ptSeasonNormal->set_partition_id(ACCOUNT_PARTITION_SEASON_NORMAL);
	ptSeasonNormal->set_accepted_license_bits(0);
	*ptSeasonNormal->mutable_saved_attributes() = tDefault.saved_attributes();
	*ptSeasonNormal->mutable_crafter_data() = tDefault.deprecated_crafter_normal_data();
	ptSeasonNormal->mutable_gold_id_deprecated()->set_id_high(0);
	ptSeasonNormal->mutable_gold_id_deprecated()->set_id_low(0);
	ptSeasonNormal->set_alt_level(0);
	*ptSeasonNormal->mutable_items() = D3::Items::ItemList::default_instance();


	D3::Account::AccountPartition * ptSeasonHardcore = tWriteData.mutable_account_data()->add_partitions();
	ptSeasonHardcore->set_partition_id(ACCOUNT_PARTITION_SEASON_HARDCORE);
	ptSeasonHardcore->set_accepted_license_bits(0);
	*ptSeasonHardcore->mutable_saved_attributes() = tDefault.saved_attributes();
	*ptSeasonHardcore->mutable_crafter_data() = tDefault.deprecated_crafter_normal_data();
	ptSeasonHardcore->mutable_gold_id_deprecated()->set_id_high(0);
	ptSeasonHardcore->mutable_gold_id_deprecated()->set_id_low(0);
	ptSeasonHardcore->set_alt_level(0);
	*ptSeasonHardcore->mutable_items() = D3::Items::ItemList::default_instance();
	// Lines 337-358 leave no code in the original; closing-brace row 359:1 (tDefault destructor).





















}








// DWARF decl 369, DW_INL_inlined into ConvertParagonLevels only; mirrors the season AddXp.
static void _AccountMigrate_107_108_AddXp(
	const ExperienceTableAlt & tExpTableAlt,
	uint32 & accountAltLevel,
	uint64 & accountXpNext,
	uint64 xpGained)
{
	while (xpGained)
	{
		if (xpGained < accountXpNext)
		{
			accountXpNext -= xpGained;
			return;
		}

		xpGained -= accountXpNext;

		accountAltLevel += 1;
		accountXpNext = tExpTableAlt.ptExperienceLevelsAlt[Math::Min<uint32>(accountAltLevel, HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP;
	}
}



// DWARF decl 393, DW_INL_inlined into Migrate_107_108 only.
static void _AccountMigrate_107_108_ConvertParagonLevels(
	const D3::AccountMigration::ReadData_107 & tReadData,
	D3::AccountMigration::WriteData_108 & tWriteData)
{
	SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT);
	ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
	const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;

	uint32 paragonLevelNormal = 0;
	uint32 paragonLevelHardcore = 0;
	uint64 altXpNextNormal = tExpTableAlt.ptExperienceLevelsAlt[0].nDeltaXP;
	uint64 altXpNextHardcore = altXpNextNormal;

	for (int32 i = 0; i < tReadData.heroes_data_size(); ++i)
	{
		const D3::AccountMigration::HeroReadData_107 & tHeroData = tReadData.heroes_data(i);
		// Inference: deleted() is evaluated at the loop latch; deleted heroes are skipped.
		if (tHeroData.deleted())
			continue;




		int32 altLevel = AccountDefinitionSavedAttributeGetInt(tHeroData.saved_attributes(), ATTRIBUTE_ALT_LEVEL, 0);



		// Inference: threshold 100 from Digest_Flags_PARAGON_100_VANILLA_FEAT; the flag set (425) has no constant evidence.
		if (altLevel >= 100)
		{

			uint32 uFlags = tWriteData.account_data().flags();
			uFlags |= (1u << D3::Account::Digest_Flags_PARAGON_100_VANILLA_FEAT);
			tWriteData.mutable_account_data()->set_flags(uFlags);
		}



		for (int level = 0; level < altLevel; level++)
		{
			int32 deltaXp = tExpTableAlt.ptExperienceLevelsAlt[level].nDeltaXPClassic;
			// Inference 435/459: hardcore split by player flags (no accessor DIE survives; fallen heroes 469-482 are hardcore-only).
			if (!(tHeroData.player_flags() & (1u << PLAYERFLAG_HARDCORE)))
			{


				_AccountMigrate_107_108_AddXp(tExpTableAlt, paragonLevelNormal, altXpNextNormal, deltaXp);
			}
			else
			{
				_AccountMigrate_107_108_AddXp(tExpTableAlt, paragonLevelHardcore, altXpNextHardcore, deltaXp);
			}
		}





		int32 nDeltaXPClassic = tExpTableAlt.ptExperienceLevelsAlt[Math::Min<int32>(altLevel, HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXPClassic;
		// Inference 455: DWARF records both Math::Max arguments as non-constant; operands are inferred.
		int32 altExpNext = AccountDefinitionSavedAttributeGetInt(tHeroData.saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, 0);
		altExpNext = Math::Max(0, altExpNext);
		int32 partialXp = nDeltaXPClassic - Math::Max(altExpNext, nDeltaXPClassic - nDeltaXPClassic);



		if (tHeroData.player_flags() & (1u << PLAYERFLAG_HARDCORE))
		{
			_AccountMigrate_107_108_AddXp(tExpTableAlt, paragonLevelHardcore, altXpNextHardcore, partialXp);
		}
		else
		{
			_AccountMigrate_107_108_AddXp(tExpTableAlt, paragonLevelNormal, altXpNextNormal, partialXp);
		}
	}

    for (int32 i = 0; i < tReadData.fallen_heroes_data_size(); ++i)
    {
        const D3::AccountMigration::FallenHeroReadData_107 & tHeroData = tReadData.fallen_heroes_data(i);
        int32 altLevel = tHeroData.alt_level();


        for (int level = 0; level < altLevel; level++)
        {
            int32 deltaXp = tExpTableAlt.ptExperienceLevelsAlt[level].nDeltaXPClassic;
            // Inference: fallen heroes are hardcore-only, one AddXp (480).

            _AccountMigrate_107_108_AddXp(tExpTableAlt, paragonLevelHardcore, altXpNextHardcore, deltaXp);
        }
    }


	D3::Account::AccountPartition * ptAccountPartitionNormal = tWriteData.mutable_account_data()->mutable_partitions(ACCOUNT_PARTITION_NORMAL);
	D3::Account::AccountPartition * ptAccountPartitionHardcore = tWriteData.mutable_account_data()->mutable_partitions(ACCOUNT_PARTITION_HARDCORE);

	tWriteData.mutable_account_profile_data()->set_normal_alt_level(paragonLevelNormal);
	tWriteData.mutable_account_profile_data()->set_normal_alt_exp(altXpNextNormal);
	AccountDefinitionSavedAttributeWriteInt(*ptAccountPartitionNormal->mutable_saved_attributes(), ATTRIBUTE_ALT_LEVEL, paragonLevelNormal);
	AccountDefinitionSavedAttributeWriteUInt64(*ptAccountPartitionNormal->mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI, altXpNextNormal);

	tWriteData.mutable_account_profile_data()->set_hardcore_alt_level(paragonLevelHardcore);
	tWriteData.mutable_account_profile_data()->set_hardcore_alt_exp(altXpNextHardcore);
	AccountDefinitionSavedAttributeWriteInt(*ptAccountPartitionHardcore->mutable_saved_attributes(), ATTRIBUTE_ALT_LEVEL, paragonLevelHardcore);
	AccountDefinitionSavedAttributeWriteUInt64(*ptAccountPartitionHardcore->mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI, altXpNextHardcore);

	ptAccountPartitionNormal->set_alt_level(paragonLevelNormal);
	ptAccountPartitionHardcore->set_alt_level(paragonLevelHardcore);
}
























































































































static void _AccountMigrate_108_109_UpdatePartitions(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData)
{


    *tWriteData.mutable_account_data()->mutable_partition_0_stash_icons() = tReadData.account_data().deprecated_stash_icons_normal();
    *tWriteData.mutable_account_data()->mutable_partition_1_stash_icons() = tReadData.account_data().deprecated_stash_icons_hardcore();


    *tWriteData.mutable_account_data()->mutable_partition_0_saved_attributes() = tReadData.account_data().partition_0_saved_attributes();






    *tWriteData.mutable_account_data()->mutable_partition_1_saved_attributes() = tReadData.account_data().partition_1_saved_attributes();
}









static void _AccountMigrate_108_109_CreateMinis(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData)
{



    for (int i = 0; i < tReadData.heroes_data_size(); i++)
    {
        const D3::AccountMigration::HeroReadData_108 & tHeroReadData = tReadData.heroes_data(i);
        D3::Profile::HeroMiniProfile * mini = tWriteData.add_minis();
        mini->set_hero_id(tHeroReadData.hero_id().id_low());
        mini->set_hero_name(tHeroReadData.hero_name());
        mini->set_hero_gbid_class(tHeroReadData.gbid_class());
        mini->set_hero_level(tHeroReadData.level());
        mini->set_hero_flags(tHeroReadData.player_flags());
        mini->mutable_hero_visual_equipment()->CopyFrom(tHeroReadData.visual_equipment());
    }
}


static void _AccountMigrate_109_110_UpdateAccountAttributes(const D3::AccountMigration::ReadData_109 & tReadData, D3::AccountMigration::WriteData_110 & tWriteData)
{
    *tWriteData.mutable_account_data()->mutable_saved_attributes() = tReadData.account_data().saved_attributes();
    *tWriteData.mutable_account_data()->mutable_saved_attributes_hardcore() = tReadData.account_data().saved_attributes_hardcore();
}
}  // namespace AccountMigration
}  // namespace OnlineService








void ConsoleMigrateCurrency_110_111(D3::Account::AccountPartition & tPartition)
{
    D3::Items::CurrencySavedData * ptData = tPartition.mutable_currency_data();


    for (int i = 0; i < ptData->currency_size(); i++)
    {

        if (ptData->currency(i).gbid_deprecated() == GlobalGBIDGet(GLOBALGBID_CURRENCY_GOLD))
        {
            ptData->mutable_currency(i)->set_id(CURRENCY_GOLD);
        }
        else if (ptData->currency(i).gbid_deprecated() == GlobalGBIDGet(GLOBALGBID_CURRENCY_BLOODSHARDS))
        {
            ptData->mutable_currency(i)->set_id(CURRENCY_BLOODSHARDS);
        }
    }
}

namespace OnlineService { namespace AccountMigration {
static void _AccountMigrate_110_111_MigrateGold(
    D3::CS::Snapshot & tSnapshot)
{


    for (int32 partitionId = 0; partitionId < tSnapshot.definition().partitions_size(); partitionId++)
    {
        D3::Account::AccountPartition & tPartition = *tSnapshot.mutable_definition()->mutable_partitions(partitionId);

        if (tPartition.has_currency_data())
        {
            ConsoleMigrateCurrency_110_111(tPartition);
        }


        blz::vector<int32> tItemIndexesToDelete;

        for (int32 itemIndex = 0; itemIndex < tPartition.items().items_size(); itemIndex++)
        {
            const D3::Items::SavedItem & tSavedItem = tPartition.items().items(itemIndex);
            if (tSavedItem.item_slot() == SAVED_SLOT_PLAYER_GOLD_DEPRECATED)
            {
                int64 nGoldTotal = CurrencyDataGetCount(CURRENCY_GOLD, tPartition.currency_data());
                nGoldTotal += tSavedItem.generator().stack_size();
                CurrencyDataSetCount(CURRENCY_GOLD, *tPartition.mutable_currency_data(), nGoldTotal);

                tItemIndexesToDelete.push_back(itemIndex);
            }
        }


        for (size_t i = 0; i < tItemIndexesToDelete.size(); i++)
        {
            int32 indexToDelete = tItemIndexesToDelete[i];
            tPartition.mutable_items()->mutable_items()->SwapElements(indexToDelete, tPartition.items().items_size() - 1);
            tPartition.mutable_items()->mutable_items()->RemoveLast();
        }
    }
}




































static void _AccountMigrate_111_112_MigrateBlacksmithLevels(D3::CS::Snapshot & tSnapshot)
{




    for (int32 partitionId = 0; partitionId < tSnapshot.definition().partitions_size(); partitionId++)
    {

        D3::Account::AccountPartition * tPartition = tSnapshot.mutable_definition()->mutable_partitions(partitionId);
        if (tPartition->has_crafter_data() && tPartition->crafter_data().crafter_data_size() > 0)
        {
            D3::ItemCrafting::CrafterData * tBlacksmithData = tPartition->mutable_crafter_data()->mutable_crafter_data(0);
            int32 nOldLevel = tBlacksmithData->level();
            if (nOldLevel > 0)
            {
                int32 nNewLevel = (nOldLevel - 1) / 5;
                if ((nOldLevel - 1) % 5 > 0)
                {
                    nNewLevel++;
                }

                nNewLevel++;
                if (nNewLevel == 12 && nOldLevel != 56)
                {
                    nNewLevel = 11;
                }

                tBlacksmithData->set_level(nNewLevel);
            }
        }
    }
}




































































































static void _AccountMigrate_114_115_HealAccountWideTransmogs(
    const D3::AccountMigration::ReadData_114 & tReadData,
    D3::AccountMigration::WriteData_115 & tWriteData)
{
    const D3::AccountMigration::AccountData_114_115 & tAccountReadData = tReadData.account_data();
    D3::AccountMigration::AccountData_114_115 & tAccountWriteData = *tWriteData.mutable_account_data();

    const D3::Account::SavedDefinition & srcAccountDefinition = tAccountReadData.snapshot().definition();
    D3::Account::SavedDefinition & healedAccountDefinition = *tAccountWriteData.mutable_snapshot()->mutable_definition();


    tAccountWriteData = tAccountReadData;

    GameAccountHandle handle(
        tAccountReadData.snapshot().account_id().id_low(),
        PROGRAM_D3,
        1);






    HealTransmogs(srcAccountDefinition, healedAccountDefinition, handle, "[AccountMigration]");


    *tWriteData.mutable_hero_settings_data() = tReadData.hero_settings_data();
}


static void _AccountMigrate_115_116_CheckSoloGriftCompletion(D3::CS::Snapshot & tSnapshot)
{
    tSnapshot.mutable_definition()->mutable_digest()->set_completed_solo_rift(false);



    for (const D3::Account::AccountPartition & partition : tSnapshot.definition().partitions())
    {
        if (AccountDefinitionSavedAttributeGetInt(partition.saved_attributes(), ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL, 0) > 0)
        {
            tSnapshot.mutable_definition()->mutable_digest()->set_completed_solo_rift(true);
            break;
        }
    }
}

static void SeasonMigrateCurrencies(
    SeasonMigrationData & tData)
{
    D3::Items::CurrencySavedData & nonSeasonCurrencies = *tData.m_ptNonSeasonPartition->mutable_currency_data();
    D3::Items::CurrencySavedData & seasonCurrencies = *tData.m_ptSeasonPartition->mutable_currency_data();

    typedef blz::unordered_map<int, D3::Items::CurrencyData> CurrencyMap;
    CurrencyMap currencyMap;


    for (int i = 0; i < nonSeasonCurrencies.currency_size(); i++)
    {
        D3::Items::CurrencyData & mapCurrency = currencyMap[nonSeasonCurrencies.currency(i).id()];
        mapCurrency = nonSeasonCurrencies.currency(i);
        mapCurrency.set_data_persist_flags(1 << DPF_PERSISTED);
    }





    for (int i = 0; i < seasonCurrencies.currency_size(); i++)
    {
        D3::Items::CurrencyData & seasonalCurrency = *seasonCurrencies.mutable_currency(i);
        auto it = currencyMap.find(seasonalCurrency.id());
        if (it != currencyMap.end())
        {

            it->second.set_count(it->second.count() + seasonalCurrency.count());
            uint32 uFlags = it->second.data_persist_flags();
            uFlags |= 1 << DPF_DIRTY_PERSIST;
            it->second.set_data_persist_flags(uFlags);
        }
        else
        {
            D3::Items::CurrencyData & mapCurrency = currencyMap[seasonalCurrency.id()];
            mapCurrency = seasonalCurrency;
            mapCurrency.set_data_persist_flags(1 << DPF_DIRTY_PERSIST);
        }



        seasonalCurrency.set_count(0);
        uint32 uFlags = seasonalCurrency.data_persist_flags();
        uFlags |= 1 << DPF_DIRTY_PERSIST;
        seasonalCurrency.set_data_persist_flags(uFlags);
    }


    nonSeasonCurrencies.clear_currency();

    for (CurrencyMap::const_iterator it = currencyMap.begin(); it != currencyMap.end(); it++)
    {
        D3::Items::CurrencyData * ptCurrency = nonSeasonCurrencies.add_currency();
        *ptCurrency = it->second;
    }
}


static void SeasonMigrateCrafterData(
    SeasonMigrationData & tData)
{
    D3::ItemCrafting::CrafterSavedData & tNonSeasonCrafterSavedData = *tData.m_ptNonSeasonPartition->mutable_crafter_data();
    const D3::ItemCrafting::CrafterSavedData & tSeasonCrafterSavedData = tData.m_ptSeasonPartition->crafter_data();

    typedef blz::unordered_set<int> GBIDSet;
    GBIDSet gbids;

    // Inference: original iterates four crafter records (compare immediate 4).
    for (int32 i = 0; i < 4; i++)
    {
        CrafterData tNonSeasonCrafterData;
        CrafterData tSeasonCrafterData;

        if (i < tNonSeasonCrafterSavedData.crafter_data_size())
        {
            tNonSeasonCrafterData.Import(tNonSeasonCrafterSavedData.crafter_data(i));
        }
        else
        { // Inference: missing nonseason crafter records begin with zero recipes and level.
            tNonSeasonCrafterData.nNumRecipes = 0;
            tNonSeasonCrafterData.nLevel = 0;
            tNonSeasonCrafterSavedData.add_crafter_data();
        }
        if (i < tSeasonCrafterSavedData.crafter_data_size())
        {
            tSeasonCrafterData.Import(tSeasonCrafterSavedData.crafter_data(i));
        }
        else
        { // Inference: absent seasonal crafter data uses zero recipes and level.
            tSeasonCrafterData.nNumRecipes = 0;
            tSeasonCrafterData.nLevel = 0;
        }

        if (tSeasonCrafterData.nNumRecipes > 0)
        {
            gbids.clear();


            for (int32 j = 0; j < tNonSeasonCrafterData.nNumRecipes; j++)
            {
                gbids.insert(tNonSeasonCrafterData.gbidRecipes[j]);
            }
            for (int32 j = 0; j < tSeasonCrafterData.nNumRecipes; j++)
            {
                gbids.insert(tSeasonCrafterData.gbidRecipes[j]);
            }


            GBIDSet::const_iterator it;
            int32 j = 0;
            for (it = gbids.begin(); it != gbids.end(); it++)
            {
                tNonSeasonCrafterData.gbidRecipes[j++] = *it;
            }
            tNonSeasonCrafterData.nNumRecipes = gbids.size();
        }


        tNonSeasonCrafterData.nLevel = Math::Max(tNonSeasonCrafterData.nLevel, tSeasonCrafterData.nLevel);


        tNonSeasonCrafterData.Export(*tNonSeasonCrafterSavedData.mutable_crafter_data(i));
    }


    if (tSeasonCrafterSavedData.has_transmog_data())
    {
        gbids.clear();

        CrafterTransmogData tSeasonTransmogData;
        CrafterTransmogData tNonSeasonTransmogData;

        tNonSeasonTransmogData.Import(tNonSeasonCrafterSavedData.transmog_data(), NULL);
        tSeasonTransmogData.Import(tSeasonCrafterSavedData.transmog_data(), NULL);

        for (int32 j = 0; j < tNonSeasonTransmogData.nUnlockedTransmogs; j++)
        {
            gbids.insert(tNonSeasonTransmogData.gbidUnlockedTransmogs[j]);
        }
        for (int32 j = 0; j < tSeasonTransmogData.nUnlockedTransmogs; j++)
        {
            gbids.insert(tSeasonTransmogData.gbidUnlockedTransmogs[j]);
        }


        GBIDSet::const_iterator it;
        int32 j = 0;
        for (it = gbids.begin(); it != gbids.end(); it++, j++)
        {
            tNonSeasonTransmogData.gbidUnlockedTransmogs[j] = *it;
        }
        tNonSeasonTransmogData.nUnlockedTransmogs = gbids.size();


        tNonSeasonTransmogData.Export(*tNonSeasonCrafterSavedData.mutable_transmog_data(), NULL);
    }


    if (tSeasonCrafterSavedData.has_extracted_legendaries_data())
    {
        gbids.clear();

        ExtractedLegendariesData tSeasonExtractedLegendaryData;
        ExtractedLegendariesData tNonSeasonExtractedLegendaryData;

        tNonSeasonExtractedLegendaryData.Import(tNonSeasonCrafterSavedData.extracted_legendaries_data());
        tSeasonExtractedLegendaryData.Import(tSeasonCrafterSavedData.extracted_legendaries_data());

        for (int32 j = 0; j < tNonSeasonExtractedLegendaryData.nExtractedLegendariesCount; j++)
        {
            gbids.insert(tNonSeasonExtractedLegendaryData.gbidExtractedLegendaries[j]);
        }
        for (int32 j = 0; j < tSeasonExtractedLegendaryData.nExtractedLegendariesCount; j++)
        {
            gbids.insert(tSeasonExtractedLegendaryData.gbidExtractedLegendaries[j]);
        }


        GBIDSet::const_iterator it;
        int32 j = 0;
        for (it = gbids.begin(); it != gbids.end(); it++, j++)
        {
            tNonSeasonExtractedLegendaryData.gbidExtractedLegendaries[j] = *it;
        }
        tNonSeasonExtractedLegendaryData.nExtractedLegendariesCount = gbids.size();


        tNonSeasonExtractedLegendaryData.Export(*tNonSeasonCrafterSavedData.mutable_extracted_legendaries_data());
    }


    tData.m_ptSeasonPartition->clear_crafter_data();
}
} }

static void AddXp(
    const ExperienceTableAlt & tExpTableAlt,
    uint32 & accountAltLevel,
    uint64 & accountXpNext,
    uint64 xpGained)
{
    while (xpGained)
    {
        if (xpGained < accountXpNext)
        {
            accountXpNext -= xpGained;
            // Inference: the partial-level branch exits this helper immediately.
            return;
        }

        xpGained -= accountXpNext;
        accountAltLevel += 1;
        accountXpNext = tExpTableAlt.ptExperienceLevelsAlt[Math::Min<uint32>(accountAltLevel, HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP;
    }
}

namespace OnlineService { namespace AccountMigration {
BOOL SeasonMigratePartition(
    SeasonMigrationData & tData)
{
    D3::Account::AccountPartition & tNonSeasonPartition = *tData.m_ptNonSeasonPartition;
    D3::Account::AccountPartition & tSeasonPartition = *tData.m_ptSeasonPartition;


    SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT);
    ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
    const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;

    uint32 paragonLevelSeason = tSeasonPartition.alt_level();
    uint32 paragonLevelNonSeason = tNonSeasonPartition.alt_level();
    uint64 paragonXpNextNonSeason = AccountDefinitionSavedAttributeGetUInt64(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI);


    for (uint level = 0; level < paragonLevelSeason; level++)
    {
        uint64 deltaXp = tExpTableAlt.ptExperienceLevelsAlt[Math::Min<uint32>(level, HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP;


        AddXp(tExpTableAlt, paragonLevelNonSeason, paragonXpNextNonSeason, deltaXp);
    }


    uint64 paragonXpNextSeason = AccountDefinitionSavedAttributeGetUInt64(*tSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI);
    if (paragonXpNextSeason != 0)
    {
        uint64 remainingXpSeason = tExpTableAlt.ptExperienceLevelsAlt[Math::Min<uint32>(paragonLevelSeason, HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP - paragonXpNextSeason;
        AddXp(tExpTableAlt, paragonLevelNonSeason, paragonXpNextNonSeason, remainingXpSeason);
    }
    // Inference: clamp spelling follows the original compare against HERO_ALT_LEVEL_MAX.
    paragonLevelNonSeason = Math::Min<uint32>(paragonLevelNonSeason, HERO_ALT_LEVEL_MAX);







    if (tNonSeasonPartition.alt_level() != paragonLevelNonSeason)
    {

        tData.m_republishPresence = true;

    }
    tNonSeasonPartition.set_alt_level(paragonLevelNonSeason);
    AccountDefinitionSavedAttributeWriteInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_LEVEL, paragonLevelNonSeason);
    AccountDefinitionSavedAttributeWriteUInt64(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI, paragonXpNextNonSeason);



    if (paragonXpNextNonSeason == 0)
    {
        AccountDefinitionSavedAttributeRemoveInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO);
        AccountDefinitionSavedAttributeRemoveInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI);
    }



    uint64 acceptedLicenseBitsSeason = tSeasonPartition.accepted_license_bits();
    uint64 acceptedLicenseBitsNonSeason = tNonSeasonPartition.accepted_license_bits();
    tNonSeasonPartition.set_accepted_license_bits(acceptedLicenseBitsSeason | acceptedLicenseBitsNonSeason);

    SeasonMigrateCurrencies(tData);
    SeasonMigrateCrafterData(tData);


    uint32 uSeasonalTabs = AccountDefinitionSavedAttributeGetInt(tSeasonPartition.saved_attributes(), ATTRIBUTE_STASH_TABS_PURCHASED_WITH_GOLD, 0);
    uint32 uNonSeasonalTabs = AccountDefinitionSavedAttributeGetInt(tNonSeasonPartition.saved_attributes(), ATTRIBUTE_STASH_TABS_PURCHASED_WITH_GOLD, 0);
    if (uSeasonalTabs > uNonSeasonalTabs)
    {
        AccountDefinitionSavedAttributeWriteInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_STASH_TABS_PURCHASED_WITH_GOLD, uSeasonalTabs);
    }



    uint32 uSeasonalStashSlots = AccountDefinitionSavedAttributeGetInt(tSeasonPartition.saved_attributes(), ATTRIBUTE_SHARED_STASH_SLOTS, 0);
    uint32 uNonSeasonalStashSlots = AccountDefinitionSavedAttributeGetInt(tNonSeasonPartition.saved_attributes(), ATTRIBUTE_SHARED_STASH_SLOTS, 0);
    if (uSeasonalStashSlots > uNonSeasonalStashSlots)
    {
        AccountDefinitionSavedAttributeWriteInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_SHARED_STASH_SLOTS, uSeasonalStashSlots);
    }




    uint32 uSeasonalHighestSoloRiftLevel = AccountDefinitionSavedAttributeGetInt(tSeasonPartition.saved_attributes(), ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL, 0);
    uint32 uNonSeasonalHighestSoloRiftLevel = AccountDefinitionSavedAttributeGetInt(tNonSeasonPartition.saved_attributes(), ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL, 0);
    if (uSeasonalHighestSoloRiftLevel > uNonSeasonalHighestSoloRiftLevel)
    {
        AccountDefinitionSavedAttributeWriteInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL, uSeasonalHighestSoloRiftLevel);
    }



    uint32 uSeasonalHighestUnlockedRiftLevel = AccountDefinitionSavedAttributeGetInt(tSeasonPartition.saved_attributes(), ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL, 0);
    uint32 uNonSeasonalHighestUnlockedRiftLevel = AccountDefinitionSavedAttributeGetInt(tNonSeasonPartition.saved_attributes(), ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL, 0);
    if (uSeasonalHighestUnlockedRiftLevel > uNonSeasonalHighestUnlockedRiftLevel)
    {
        AccountDefinitionSavedAttributeWriteInt(*tNonSeasonPartition.mutable_saved_attributes(), ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL, uSeasonalHighestUnlockedRiftLevel);
    }


    tSeasonPartition.clear_saved_attributes();
    tSeasonPartition.clear_stash_icons();
    tSeasonPartition.set_accepted_license_bits(0);
    tSeasonPartition.set_alt_level(0);
    tSeasonPartition.set_flags(0);
    // Inference: every completed migration returns the original constant TRUE.
    return TRUE;
}
} }
namespace OnlineService { namespace AccountMigration {

BOOL SeasonCleanSeasonalAchievements(
    D3::Achievements::Snapshot & tAchievementsSnapshot)
{


    // Inference: the original change accumulator starts false and is set only for removed records.
    bool bSnapshotWasCleaned = false;
    D3::Achievements::Snapshot tCleanSnapshot;
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    for (const D3::Achievements::AchievementUpdateRecord & tAchv : tAchievementsSnapshot.achievement_snapshot())
    {
        const OnlineService::AchievementDefinition * ptAchievementDefinition = tAchievements.GetAchievementDefinition(tAchv.achievement_id());
#line 1299
        // Inference: mask 1 is the normal-account partition bit; missing definitions retain their records.
        if (!ptAchievementDefinition ||
            ptAchievementDefinition->m_partitionMask == (1u << ACCOUNT_PARTITION_NORMAL))
        {
            *tCleanSnapshot.add_achievement_snapshot() = tAchv;
        }
        else
        {
            bSnapshotWasCleaned = true;
        }
    }

    for (const D3::Achievements::CriteriaUpdateRecord & tCriteria : tAchievementsSnapshot.criteria_snapshot())
    {
        OnlineService::CriteriaId idCriteria = AchievementsEngine::Make64BitCriteriaId(AchievementsEngine::ExtractCriteriaId32(tCriteria.criteria_id_32_and_flags_8()));
        const OnlineService::CriteriaDefinition * ptCriteriaDefinition = tAchievements.GetCriteriaDefinition(idCriteria);
        const OnlineService::AchievementDefinition * ptAchievementDefinition = ptCriteriaDefinition ? tAchievements.GetAchievementDefinition(ptCriteriaDefinition->m_parentAchievementId) : NULL;
#line 1315
        // Inference: mask 1 is the normal-account partition bit; missing definitions retain their records.
        if (!ptAchievementDefinition ||
            ptAchievementDefinition->m_partitionMask == (1u << ACCOUNT_PARTITION_NORMAL))
        {
            *tCleanSnapshot.add_criteria_snapshot() = tCriteria;
        }
        else
        {
            bSnapshotWasCleaned = true;
        }
    }

    if (bSnapshotWasCleaned)
    {
        tAchievementsSnapshot.Swap(&tCleanSnapshot);
    }

    return bSnapshotWasCleaned;
}
} }

namespace OnlineService { namespace AccountMigration {
#line 503
// Inference 508/509: declarators written Level, Normal, Hardcore (DWARF lists them reversed); this order gives the original select order.
static void _AccountMigrate_107_108_ConvertHeroes(
    const D3::AccountMigration::ReadData_107 & tReadData,
    D3::AccountMigration::WriteData_108 & tWriteData)
{
    int32 nHighestLevel = 0;
#line 508
    int32 nHighestNormalLevel = 0;
#line 508
    int32 nHighestHardcoreLevel = 0;
    BOOL bCompletedNormal = FALSE;
#line 509
    BOOL bCompletedHardcore = FALSE;
    for (int32 i = 0; i < tReadData.heroes_data_size(); ++i)
    {
        const D3::AccountMigration::HeroReadData_107 & tHeroReadData = tReadData.heroes_data(i);
        D3::AccountMigration::HeroWriteData_108 & tHeroWriteData = *tWriteData.add_heroes_data();
        *tHeroWriteData.mutable_hero_id() = tHeroReadData.hero_id();
        *tHeroWriteData.mutable_digest_ui() = tHeroReadData.digest_ui();


        // Inference: positive deprecated difficulty indicates completed campaign; original writes ACT_MAX_CAMPAIGN (400).
        if (tHeroReadData.digest_ui().highest_unlocked_difficulty_deprecated() > 0)
        {
            tHeroWriteData.mutable_digest_ui()->set_highest_unlocked_act(ACT_MAX_CAMPAIGN);
            if (tHeroReadData.player_flags() & (1u << PLAYERFLAG_HARDCORE))
            {
                bCompletedHardcore = TRUE;
            }
            else
            {
                bCompletedNormal = TRUE;
            }
        }


        typedef blz::unordered_map<int, int> QuestSNOMap;
        QuestSNOMap questMap;

        for (int32 j = 0; j < tHeroReadData.quest_history_size(); ++j)
        {
            const D3::Hero::QuestHistoryEntry & tQuestEntry = tHeroReadData.quest_history(j);

            QuestSNOMap::const_iterator it = questMap.find(tQuestEntry.sno_quest());
            if (it == questMap.end() || !tQuestEntry.has_highest_played_quest_step())
            {
                // Inference: absent map entry or absent explicit step uses the protobuf's step value.
                questMap[tQuestEntry.sno_quest()] = tQuestEntry.highest_played_quest_step();
            }
            else
            {
                // Inference: preserve completion/identical steps and advance only along reachable quest steps.
                if (it->second != QUEST_ANY_COMPLETION_STEP_UID &&
                    it->second != tQuestEntry.highest_played_quest_step())
                {

                    if (QuestStepIsReachableByStep(tQuestEntry.sno_quest(), it->second, tQuestEntry.highest_played_quest_step()))
                    {






                        questMap[tQuestEntry.sno_quest()] = tQuestEntry.highest_played_quest_step();
                    }
                }
            }
        }


        for (QuestSNOMap::const_iterator it = questMap.begin(); it != questMap.end(); it++)
        {
            D3::Hero::QuestHistoryEntry & tQuestHistoryEntry = *tHeroWriteData.add_quest_history();
            tQuestHistoryEntry.set_sno_quest(it->first);
            if (it->second != QUEST_ANY_COMPLETION_STEP_UID)
            {
                tQuestHistoryEntry.set_highest_played_quest_step(it->second);
            }
        }

        if (!(tHeroReadData.player_flags() & (1u << PLAYERFLAG_HARDCORE)))
        {
            nHighestNormalLevel = Math::Max(nHighestNormalLevel, tHeroReadData.level());
        }
        else
        {
            nHighestHardcoreLevel = Math::Max(nHighestHardcoreLevel, tHeroReadData.level());
        }

    }
    tWriteData.mutable_account_profile_data()->set_highest_hero_level(nHighestNormalLevel);
    // Inference: original threshold 60 enables both normal difficulty flags; completed campaign enables master alone.
    if (nHighestNormalLevel >= 60)
    {
        uint32 dwFlags = tWriteData.account_data().flags();
        dwFlags |= (1u << D3::Account::Digest_Flags_MASTER_DIFFICULTY_UNLOCKED);
        dwFlags |= (1u << D3::Account::Digest_Flags_TORMENT_DIFFICULTY_UNLOCKED);
        tWriteData.mutable_account_data()->set_flags(dwFlags);
    }
    else if (bCompletedNormal)
    {
        uint32 dwFlags = tWriteData.account_data().flags();
        dwFlags |= (1u << D3::Account::Digest_Flags_MASTER_DIFFICULTY_UNLOCKED);
        tWriteData.mutable_account_data()->set_flags(dwFlags);
    }
    // Inference: the same threshold independently enables hardcore difficulty flags.
    if (nHighestHardcoreLevel >= 60)
    {
        uint32 dwFlags = tWriteData.account_data().flags();
        dwFlags |= (1u << D3::Account::Digest_Flags_HARDCORE_MASTER_DIFFICULTY_UNLOCKED);
        dwFlags |= (1u << D3::Account::Digest_Flags_HARDCORE_TORMENT_DIFFICULTY_UNLOCKED);
        tWriteData.mutable_account_data()->set_flags(dwFlags);
    }
    else if (bCompletedHardcore)
    {
        uint32 dwFlags = tWriteData.account_data().flags();
        dwFlags |= (1u << D3::Account::Digest_Flags_HARDCORE_MASTER_DIFFICULTY_UNLOCKED);
        tWriteData.mutable_account_data()->set_flags(dwFlags);
    }
}
} }

#include "../../Sigma/DataStructures/XVar.h"
static XVarScope s_scopeBase("AccountMigration", NULL);
static XVarScope s_scope114(s_scopeBase, "114Migration");
XVarUint32 s_var114ExtractedLegendaryHealUnixTime(s_scope114, "ExtractedLegendaryHealUnixTime", 0);







namespace OnlineService {
namespace AccountMigration {
static void _AccountMigrate_113_114_MigrateExtractedLegendaries(const D3::CS::Snapshot & tOldSnapshot, D3::CS::Snapshot & tSnapshot)
{
    blz::unordered_set<int> gbids;
    blz::unordered_set<int> hardcoreGbids;
    for (const D3::Account::AccountPartition & partition : tSnapshot.definition().partitions())
    {
        ExtractedLegendariesData tExtractedLegendaryData;
        tExtractedLegendaryData.Import(partition.crafter_data().extracted_legendaries_data());
        for (int32 j = 0; j < tExtractedLegendaryData.nExtractedLegendariesCount; ++j)
        {
            if (partition.partition_id() == ACCOUNT_PARTITION_NORMAL)
                gbids.insert(tExtractedLegendaryData.gbidExtractedLegendaries[j]);
            else if (partition.partition_id() == ACCOUNT_PARTITION_HARDCORE)
                hardcoreGbids.insert(tExtractedLegendaryData.gbidExtractedLegendaries[j]);
        }
    }
    for (const D3::Account::AccountPartition & partition : tOldSnapshot.definition().partitions())
    {
        ExtractedLegendariesData tExtractedLegendaryData;
        tExtractedLegendaryData.Import(partition.crafter_data().extracted_legendaries_data());
        for (int32 j = 0; j < tExtractedLegendaryData.nExtractedLegendariesCount; ++j)
        {
            if (IsPartitionNormal((D3AccountPartition)partition.partition_id()))
                gbids.insert(tExtractedLegendaryData.gbidExtractedLegendaries[j]);
            else if (IsPartitionHardcore((D3AccountPartition)partition.partition_id()))
                hardcoreGbids.insert(tExtractedLegendaryData.gbidExtractedLegendaries[j]);
        }
    }
    for (int32 i = 0; i < tSnapshot.definition().partitions_size(); ++i)
    {
        D3::Account::AccountPartition & partition = *tSnapshot.mutable_definition()->mutable_partitions(i);
        ExtractedLegendariesData tExtractedLegendaryData;
        uint32 j = 0;
        if (partition.partition_id() == ACCOUNT_PARTITION_NORMAL)
        {
            for (int gbid : gbids)
                tExtractedLegendaryData.gbidExtractedLegendaries[j++] = gbid;
            tExtractedLegendaryData.nExtractedLegendariesCount = gbids.size();
            tExtractedLegendaryData.Export(*partition.mutable_crafter_data()->mutable_extracted_legendaries_data());
        }
        else if (partition.partition_id() == ACCOUNT_PARTITION_HARDCORE)
        {
            for (int gbid : hardcoreGbids)
                tExtractedLegendaryData.gbidExtractedLegendaries[j++] = gbid;
            tExtractedLegendaryData.nExtractedLegendariesCount = hardcoreGbids.size();
            tExtractedLegendaryData.Export(*partition.mutable_crafter_data()->mutable_extracted_legendaries_data());
        }
    }
}

BOOL Migrate_113_114(const D3::CS::Snapshot & tOldSnapshot, D3::CS::Snapshot & tSnapshot)
{

    if (tSnapshot.definition().version() != 113)
        return FALSE;
    tSnapshot.mutable_definition()->set_version(114);
    tSnapshot.mutable_definition()->mutable_digest()->set_version(114);
    if (tOldSnapshot.definition().partitions_size() > 0)
        _AccountMigrate_113_114_MigrateExtractedLegendaries(tOldSnapshot, tSnapshot);
    return TRUE;
}
}
}
