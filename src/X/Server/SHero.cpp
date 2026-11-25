#include "SHero.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Game.h"
#include "../Common/GlobalSNO.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// X/Server/Powers/Powers.cpp:2105 and :2089; DWARF signatures.
void PowerActorAutoPurchaseAlwaysKnownTraits(ActorCommonData & tACD);
void PowerActorAutoPurchaseStartingSkillsAndTraits(ActorCommonData & tACD);

#include "../Common/Player.h"
#include "../Common/Definitions/GameBalanceDefinition.h"

#include "SPlayer.h"
#include "ConversationMgr.h"
#include "Crafting/SItemCrafting.h"
#include "../Common/Partition.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Sigma.h"
// DWARF signatures: Hero.cpp:46; Quests.cpp:1528; SHireling.cpp:409;
// SPlayer.cpp:5006, :10451, :14687; Player.cpp:3312.
void HeroResetSavedData(Player & tPlayer);
void QuestsVersionForPlayer(HeroStateData & tStateData);
void SHirelingRestoreStableInfo(Player & tPlayer);
SPlayerConsumables & SPlayerGetConsumables(Player & tPlayer);
void SPlayerReceivedQuestStepReward(Player & tPlayer, SNO snoQuest, int32 nStepUID, DifficultyLevel eDifficulty);
void SPlayerImportDeliveredRewards(Player & player, const D3::Account::DeliveredRewards & rewards);
void PlayerQuestRewardHistoryEntryInitialize(PlayerQuestRewardHistoryEntry & tEntry);
extern const uint32 HERO_DEFINITION_VERSION_1_0_5;
extern const uint32 HERO_DEFINITION_VERSION_2_0_0;

#include "Actors/Actors.h"
#include "PlayerList.h"
// DWARF: HeroDefinition.cpp:43-44 defines the version globals above.
// DWARF signatures: SPlayer.cpp:2352, :2936, :5016, :5149, :7224, :8473;
// Player.cpp:1438; AccountDefinition.cpp:719; ActorInventory.cpp:5697; SHireling.cpp:431.
ACDID PlayerActorCreate(Player & tPlayer, const uint32 dwPlayerFlags, const WorldPlace & tPlace, const Quaternion & q, const BOOL bResetLifeStats, const D3::Hero::SavedDefinition & tHeroDefinition, const D3::Account::SavedDefinition & tAccountDefinition, const D3::Items::Mails & tMails, const uint32 dwSaveOptionBits);
void SPlayerSendBanner(const PlayerList & listDestPlayers, const Player & tPlayer);
BOOL SPlayerRemoveFromSaveList(Player & tPlayer);
void SPlayerClearEnemyList(Player & tPlayer);
void SPlayerSyncWaypointsToGame(Player & tNewPlayer);
void SPlayer_ClientLoadComplete(Player & tPlayerFrom);
void PlayerSetACD(Player & tPlayer, const ACDID idACD);
void AccountDefinition_ToDefaults(D3::Account::SavedDefinition & tDefinition);
void ACDInventoryDeleteAll(ActorCommonData & tACD);
void SHirelingSendStableInfoToClient(Player & tPlayer);

#include "../Common/ActorInventory.h"
// DWARF Items.cpp:11269.
uint64 ItemGenerateConsoleEquipmentManagerID(const ActorCommonData & tACDItem);

#include "../Common/Quests/Quests.h"
#include "../Common/VisualInventory.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/Definitions/AccountDefinition.h"
#include "SGameManager.h"
// DWARF Online.cpp:4361, SHireling.cpp:365, SPlayer.cpp:10834/10736/11351, Quests.cpp:1020.
namespace Console { namespace Online { BOOL PartyGetPlayerAccountAndHero(int32, D3::Account::SavedDefinition *, D3::Hero::SavedDefinition *); } }
void SHirelingSaveStableInfo(Player & tPlayer);
Act SPlayerGetMaxAct(const Player & tPlayer);
const OnlineService::ContentLicense * SPlayerGetContentLicense(const Player & tPlayer, OnlineService::ContentLicenseId licenseId);
BOOL SPlayerIsEligibleForQuestState(const Player & tPlayer);
QuestInstance * GetLeadingMainLineQuest();
BOOL GameTypeIsPvP(); // GameTemplate.cpp:255
BOOL GameIsOpenWorld(); // GameCommonData.cpp:311

struct PlayerRestoreLoadoutEquipment
{
    LoadoutSavedData * ptLoadouts;
};

// DWARF HeroDefinition.cpp:511; ActorInventory.cpp:5822/5908/5508/5711; AccountDefinition.cpp:567.
void HeroDefinitionSaveAttributes(ActorCommonData &, D3::AttributeSerializer::SavedAttributes &);
void ACDInventorySavedStateStore(const ActorCommonData &, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> &, uint32, bool);
void ACDHirelingInventorySavedStateStore(Player &, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> &, bool, bool);
void ACDInventorySaveItem(const ActorCommonData &, D3::Items::SavedItem &, HirelingClass, InventorySlotType);
void ACDInventoryDeleteSharedStash(ActorCommonData &);
void AccountDefinition_DigestToDefaults(D3::Account::Digest &);
namespace SHero {
static void sComposeHeroDefinition(Player &, uint32, D3::Hero::SavedDefinition &);
static void sComposeAccountDefinition(Player &, uint32, D3::Account::SavedDefinition &);
static void sResetAccountDefinition(const Player &, D3::Account::SavedDefinition &);
static void sSaveUsableItemSlot(const Player &, D3::Hero::SavedData &);
static void sSaveLoadouts(const Player &, D3::Hero::SavedDefinition &);
}
// File-scope includes and declarations for SHero::CreateActor and its helpers (lines 504-1542).
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/Networking/GameMessage.h"
#include "LuaScript/ScriptUtil.h"
#include "SGameStatsSwig.h"
#include "System/SNephalemRift.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/AI/ActorAISwig.h"
void SPlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot, BOOL bApplyCooldown);  // X/Server/SPlayer.cpp:6118
BOOL ItemCanBeAssignedToUsableItemButton(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:2352
void PlayerInventoryIterateLoadoutEquipmentSlots(const ActorCommonData & tACD, SlotSearchCallback pfnCallBack, void * pCallBackData);  // X/Common/Player.cpp:4426
BOOL SPlayerFindMatchingPotion(const ActorCommonData & tACDPlayer, GBID gbidPotion, ActorCommonData *& ptACDMatchingPotion, InventoryLocation & tMatchingPotionLoc);  // X/Server/SPlayer.cpp:15243
void SACDInventoryLoad(ACDID idACDContainer, SNO snoInventory);  // X/Common/ActorInventory.cpp:9594
SNO GameBalanceGetHeroSNO(const GBHandle & hGB, const uint32 dwPlayerFlags);  // X/Common/Definitions/GameBalanceDefinition.cpp:3313
void HeroEnsureHeroHasAllSkillAttributes(ActorCommonData & tACD, const BOOL fNewHero);  // X/Server/SHero.cpp:2317
void HeroDefinitionRestoreAttributes(ActorCommonData & tACDPlayer, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes);  // X/Common/Definitions/HeroDefinition.cpp:524
void SPlayerRestoreRunes(ActorCommonData & tACDPlayer, Player & tPlayer);  // X/Server/SPlayer.cpp:5972
void SPlayerSetLevelCap(ActorCommonData & tACDPlayer, OnlineService::ContentLicenseId eLicenseId);  // X/Server/SPlayer.cpp:10818
void ACDInventoryCreateBaseHealthPotion(ActorCommonData & tACDPlayer);  // X/Common/ActorInventory.cpp:6401
void ACDInventorySavedStateRestore(ActorCommonData & tACDPlayer, const google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems);  // X/Common/ActorInventory.cpp:6437
void SGameStatsPlayerTimedEventStart(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // X/Server/SGameStats.cpp:1571
void SACDInventoryRemoveIllegalItems(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:12937
ACDID SHirelingLoadFromStable(ACDID idACDPlayer, HirelingClass eClass);  // X/Server/SHireling.cpp:321
void AISetCalloutCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown);  // X/Server/AI/ActorAI.cpp:2873
void AccountDefinition_RestoreAttributes(ActorCommonData & tACDPlayer, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes);  // X/Common/Definitions/AccountDefinition.cpp:432
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD);  // X/Server/Experience.cpp:1293
void SPlayerFixUpCurrenciesDiscovered(ActorCommonData & tACDPlayer);  // X/Server/SPlayer.cpp:9277
BOOL PlayerAssignBaseHealthPotion(Player * ptPlayer);  // X/Common/Player.cpp:1703
uint32 SPlayerGetConsoleLicenseFlags(const Player & tPlayer);  // X/Server/SPlayer.cpp:10673
void ConsoleUpdateBonusItems(Player & tPlayer, ActorCommonData & tACDPlayer, const uint32 uConsoleLicenses);  // X/Common/Console/ConsoleLicenses.cpp:364
void ActorPowersGrantAllGlobalPowers(ActorCommonData & tACD);  // X/Server/Actors/ActorPowers.cpp:584
void SPlayerInitializeParagonPoints(Player & tPlayer);  // X/Server/SPlayer.cpp:6726
void ACDInventoryEquippedItemsChanged(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9042
void ACDInventoryUpdateSkillButtons(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:8692
void SPlayerEvaluateMultiplayerBuff();  // X/Server/SPlayer.cpp:12081
void SGameSendDungeonFinderSeedToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8521
void SGameSendDungeonFinderParticipatingPlayerCountToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8536
void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8491
void SGameSendDungeonFinderTimedEventToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8506
void ActorSetBrain(const ActorID idActor, ActorBrain nBrain, ActorSavedAI * ptSavedAI);  // X/Server/AI/ActorAI.cpp:449
BOOL SPlayerHasTradeRestrictions(const Player & tPlayer);  // X/Server/SPlayer.cpp:10716
void ACDInventoryRestoreMail(const ActorCommonData & tACD, const D3::Items::Mails & tMails);  // X/Common/ActorInventory.cpp:11511
void SPlayerRestoreCooldowns(ActorCommonData & tACDPlayer, Player & tPlayer);  // X/Server/SPlayer.cpp:5986
extern const BOOL cgfDefaultPlayerSexIsMale;  // X/Common/GlobalSNO.cpp:3037
#include "../Common/Achievements/Console/ConsoleAchievements.h"
// DWARF X/Common/Definitions/AccountDefinition.cpp:320.
void AccountDefinition_SerializeAttributes(ActorCommonData &, D3::AttributeSerializer::SavedAttributes &, AccountSerializeMode);
// DWARF X/Server/SPlayer.cpp:14710.
void SPlayerExportDeliveredRewards(const Player &, D3::Account::DeliveredRewards *);
namespace SHero {
// NON_MATCHING: 6544 vs 6544 bytes, 4 insns: only GOT-load placement around two calls differs. Inline tree now as the DWARF (Math::Min(GameGetHandicapLevel(), ...) at 336, mutable_partitions()->Add() at 2035).
BOOL Save(
    Player & tPlayer,
    PlayerSaveData & tSaveData,
    uint32 dwHeroSaveOptions)
{


    tSaveData.idGameAccount = tPlayer.tAccount.tHandle;
    tSaveData.uHeroSaveOptions = dwHeroSaveOptions;
    if (tPlayer.usHeroName[0])
    {
        tSaveData.tHeroFilename = tPlayer.usHeroName;
    }
    else
    {
        tSaveData.tHeroFilename = tPlayer.szHeroFilename;
    }

    tSaveData.tAccountFilename = tPlayer.tAccount.usName;



    if (!Console::Online::PartyGetPlayerAccountAndHero(tPlayer.nPlayerIndex, &tSaveData.tAccountDefinition, NULL))
        return FALSE;








    SHirelingSaveStableInfo(tPlayer);


    D3::Hero::SavedDefinition & tSavedDefinition = tSaveData.tHeroDefinition;
    sComposeHeroDefinition(tPlayer, dwHeroSaveOptions, tSavedDefinition);

    if ((dwHeroSaveOptions & (1u << HSO_QUIT_AFTER_SAVE_BIT)) &&
        !tPlayer.bParticipatedInVoteKick &&
        !(SGameManager::GameGet().tStartParams.dwCreationFlags & (1u << GAMECREATEPARAM_PUBLIC_BIT)))
    {

        uint32 dwNow = GameGetServerTime();
        ScopedGlobalDefinitionLockR pdefGlobals;

        uint32 dwTicksForKarmaGain = static_cast<uint32>(pdefGlobals->ptServerData->flTimeInGameForPositiveKarma * 3600.0f);

        if (dwNow > tPlayer.dwTimeStampJoined + dwTicksForKarmaGain)
        {
            ++tPlayer.nGamesWithoutKicks;
        }
    }


    D3::Account::SavedDefinition & tAccountDefinition = tSaveData.tAccountDefinition;

    if (dwHeroSaveOptions & (1u << HSO_RESET_ACCOUNT_DATA))
    {
        sResetAccountDefinition(tPlayer, tAccountDefinition);
    }
    else
    {
        sComposeAccountDefinition(tPlayer, dwHeroSaveOptions, tAccountDefinition);
    }

    if (dwHeroSaveOptions & (1u << HSO_QUIT_AFTER_SAVE_BIT))
        SPlayerDisconnect(tPlayer, static_cast<QuitReason>(0), FALSE);

    return TRUE;
}
} // namespace SHero
// DWARF X/Common/Player.h:663 (not yet present in the header subset).
enum DataPersistFlags { DPF_DIRTY_PERSIST = 0, DPF_DIRTY_CLIENTSYNC = 1, DPF_PERSIST_IN_PROGRESS = 2, DPF_PERSISTED = 3 };






































namespace SHero {

static void sComposeHeroDefinition(
    Player & tPlayer,
    uint32 dwHeroSaveOptions,
    D3::Hero::SavedDefinition & tSavedDefinition)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    tSavedDefinition.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));


    D3::Hero::Digest & tDigest = *tSavedDefinition.mutable_digest();
    tDigest.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));
    tDigest.mutable_hero_id()->set_id_high(0);
    tDigest.mutable_hero_id()->set_id_low(tPlayer.uHeroId);
    tDigest.set_hero_name(tPlayer.usHeroName, sizeof(tPlayer.usHeroName));
    tDigest.set_gbid_class(tACDPlayer.hGB.gbid);
    tDigest.set_level(tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL)));
    tDigest.set_pvp_rank(tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_PVP_RANK)));
    tDigest.set_player_flags(tPlayer.tHeroStateData.uPlayerFlags);
    tDigest.set_player_flags2(tPlayer.tHeroStateData.uPlayerFlags2);
    tDigest.set_highest_unlocked_act(tPlayer.tHeroStateData.eHighestUnlockedAct);
    tDigest.set_last_played_quest(SNO_NONE);
    tDigest.set_last_played_quest_step(-1);
    tDigest.set_time_played(tPlayer.tHeroStateData.tSavedData.uSecondsPlayed);
    tDigest.set_season_created(tPlayer.uSeasonCreated);
    tDigest.set_highest_solo_rift_completed(tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL)));

    D3::Hero::SavedData & tSavedData = *tSavedDefinition.mutable_saved_data();
    tSavedData.set_main_quest_handicap_snapshot(-1);


    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        for (int32 i = 0; i < 6; ++i)
        {
            tSavedData.add_boss_kill_flags(tPlayer.tHeroStateData.uBossKillFlags[i]);
        }
    }

    QuestInstance * pCurrentQuest = GetLeadingMainLineQuest();





    BOOL bGameOver = SPlayerGetMaxAct(tPlayer) < GameGetCurrentAct() || QuestIsGameOver();
    if (bGameOver && !GameTypeIsPvP() && !GameIsOpenWorld())
    {





        const OnlineService::ContentLicense * ptAct5ContentLicense = SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_ADVMODE_ACTV);



        if (ptAct5ContentLicense && !ptAct5ContentLicense->IsExpired())
            tDigest.set_last_played_act(static_cast<Act>(400));

        else
            tDigest.set_last_played_act(static_cast<Act>(0));


    }

    else if (SPlayerIsEligibleForQuestState(tPlayer) && !GameTypeIsPvP() && !GameIsOpenWorld())
    {



        tDigest.set_last_played_act(tPlayer.tHeroStateData.eLastPlayedAct);
        if (pCurrentQuest)
        {

            tDigest.set_last_played_quest(pCurrentQuest->snoQuest);
            tDigest.set_last_played_quest_step(pCurrentQuest->nResumableStepUID);

            QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
            if (ptQuestGlobals)
            {

                tSavedData.set_main_quest_handicap_snapshot(Math::Min(GameGetHandicapLevel(), ptQuestGlobals->nCurrentMainLineQuestHandicapLevel));

            }
        }
    }





    else
    {
        tDigest.set_last_played_act(tPlayer.tPlayerResumeStateUponJoining.eLastPlayedAct);
        tDigest.set_last_played_quest(tPlayer.tPlayerResumeStateUponJoining.snoLastPlayedQuest);
        tDigest.set_last_played_quest_step(tPlayer.tPlayerResumeStateUponJoining.nLastPlayedQuestStep);
        tSavedData.set_main_quest_handicap_snapshot(tPlayer.tPlayerResumeStateUponJoining.nHandicapLevelSnapshot);
    }






    VisualCosmeticItem * tCurrentCosmeticItems = tACDPlayer.ptVisualInventory->tEquipmentCurrent.tCosmeticItems;
    GBID gbidItemPetActual = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_COSMETIC_PET));
    GBID gbidItemPetVisual = tCurrentCosmeticItems[2].gbidItem;
    if (gbidItemPetActual != -1 && gbidItemPetVisual == -1)
        tCurrentCosmeticItems[2].gbidItem = gbidItemPetActual;


    tACDPlayer.ptVisualInventory->tEquipmentCurrent.Export(*tDigest.mutable_visual_equipment());
    tCurrentCosmeticItems[2].gbidItem = gbidItemPetVisual;


    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        for (int32 i = 0; i < 200; ++i)
        {
            if (tPlayer.tHeroStateData.tQuestHistory[i].snoQuest != SNO_NONE)
                tPlayer.tHeroStateData.tQuestHistory[i].Export(*tDigest.add_quest_history());
        }







        for (int32 i = 0; i < tPlayer.tHeroStateData.nNumQuestRewardEntries; ++i)
        {
            tPlayer.tHeroStateData.tQuestRewardHistory[i].Export(*tSavedDefinition.add_quest_reward_history());
        }





        for (int32 i = 0; i < 64; ++i)
        {
            if (tPlayer.tHeroStateData.tSavedQuest[i].snoQuest != SNO_NONE)
            {

                if (pCurrentQuest && tPlayer.tHeroStateData.tSavedQuest[i].snoQuest == pCurrentQuest->snoQuest &&
                    !SPlayerIsEligibleForQuestState(tPlayer))
                    continue;



                tPlayer.tHeroStateData.tSavedQuest[i].Export(*tSavedDefinition.add_saved_quest());
            }
        }
    }


    HeroDefinitionSaveAttributes(tACDPlayer, *tSavedDefinition.mutable_saved_attributes());


    tPlayer.tHeroStateData.tSavedData.Export(tPlayer, tACDPlayer.hGB.gbid, tSavedData, FALSE);

    if (dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA))
    {
        tSavedData.set_time_played(0);
        tSavedData.set_activated_waypoints(0);
        tSavedData.mutable_learned_lore()->clear_bitfield();
        tSavedData.mutable_learned_lore()->clear_elements();

        tSavedData.mutable_saved_conversations()->set_played_conversations_bitfield("");
        tSavedData.mutable_saved_conversations()->clear_sno_saved_conversations();
        tSavedData.mutable_saved_conversations()->clear_sno_saved_conversations_bitfield();
        tSavedData.mutable_saved_conversations()->clear_bitfield_leading_null_bytes();

        tSavedData.mutable_save_point()->set_savepoint_number(0);
        tSavedData.mutable_save_point()->set_creates_portal(0);
    }


    if (dwHeroSaveOptions & (1u << HSO_ENCODE_ITEM_DATA))
    {
        uint32 uSaveFlags = 0xffff7fffu;


        ACDInventorySavedStateStore(tACDPlayer,

            *tSavedDefinition.mutable_items()->mutable_items(), uSaveFlags, true);




        Player * ptWriteable = PlayerGetByACD(tPlayer.idPlayerACD);
        ACDHirelingInventorySavedStateStore(*ptWriteable, *tSavedDefinition.mutable_items()->mutable_items(), true, true);

    }




    else if (dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA))
    {
        uint32 uSaveFlags = 0xfff77ffeu;




        ACDInventorySavedStateStore(tACDPlayer, *tSavedDefinition.mutable_items()->mutable_items(), uSaveFlags, false);






        Player * ptWriteable = PlayerGetByACD(tPlayer.idPlayerACD);
        ACDHirelingInventorySavedStateStore(*ptWriteable, *tSavedDefinition.mutable_items()->mutable_items(), false, false);






        const PlayerSavedData * ptPlayerData = &tPlayer.tHeroStateData.tSavedData;

        if (ptPlayerData->tPotionBtn.m_annItem != ANN_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(ptPlayerData->tPotionBtn.m_annItem));
            if (ptACDItem)
            {
                const InventoryLocation * ptInvLoc = ptACDItem->TryToGetInventoryLocation();
                if (ptInvLoc && ptInvLoc->eSlot != 16)
                {
                    D3::Items::SavedItem & tSavedItem = *tSavedDefinition.mutable_items()->add_items();
                    ACDInventorySaveItem(*ptACDItem, tSavedItem, static_cast<HirelingClass>(0), ptACDItem->tLocation.tInvLocation.eSlot);
                }
            }
        }
    }

    tSavedDefinition.set_accepted_license_bits(tPlayer.uAcceptedLicenseBitsHero);

    sSaveUsableItemSlot(tPlayer, tSavedData);

    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))

        sSaveLoadouts(tPlayer, tSavedDefinition);
}
} // namespace SHero




static void sSetupStartingSkillAndButton(
    ActorCommonData & ,
    Player & tPlayer,
    const SNO snoPower,
    const int32 nSlot)
{
    if (snoPower != SNO_NONE)
    {

        SPlayerAssignToActiveSkillSlot(tPlayer, snoPower, RUNE_TYPE_INVALID, nSlot, FALSE);
    }
    else
    {
        HotbarButtonData & tButtonData = tPlayer.tHeroStateData.tSavedData.tHotbarBtnAssignments[nSlot];
        tButtonData.SetSkill(GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT), RUNE_TYPE_INVALID);
    }
}





void sRecalculateBaseAttributes(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);



    int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);

    if (nLevel > 70)
    {
        nLevel = 70;
    }

    int32 nVitality = (int32)ptACD->AttributesGetBaseFloat(ATTRIBUTE_VITALITY);
    int32 nStrength = (int32)ptACD->AttributesGetBaseFloat(ATTRIBUTE_STRENGTH);
    int32 nDexterity = (int32)ptACD->AttributesGetBaseFloat(ATTRIBUTE_DEXTERITY);
    int32 nIntelligence = (int32)ptACD->AttributesGetBaseFloat(ATTRIBUTE_INTELLIGENCE);

    PlayerClass ePlayerClass = PlayerGetPlayerClass(idACD);


    SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
    ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

    for (int32 n = 1; n < nLevel; ++n)
    {
        const AttributeAwardDefinition & tAttribAwardDef = tExpTable.ptExperienceLevels[n].tAttributeAward[ePlayerClass];

        nVitality += tAttribAwardDef.nVitality;
        nStrength += tAttribAwardDef.nStrength;
        nDexterity += tAttribAwardDef.nDexterity;
        nIntelligence += tAttribAwardDef.nIntelligence;
    }


    ptACD->AttributesSetFloat(ATTRIBUTE_VITALITY, (float)nVitality);
    ptACD->AttributesSetFloat(ATTRIBUTE_STRENGTH, (float)nStrength);
    ptACD->AttributesSetFloat(ATTRIBUTE_DEXTERITY, (float)nDexterity);
    ptACD->AttributesSetFloat(ATTRIBUTE_INTELLIGENCE, (float)nIntelligence);
}




static void sRestoreVisualInventory(
    ActorCommonData & tACDPlayer,
    const D3::Hero::VisualEquipment & tVisualEquipment)
{

    tACDPlayer.ptVisualInventory->tEquipmentCurrent.Import(tVisualEquipment);
    tACDPlayer.ModifiedMark(ACDMODIFIED_VISUAL_INVENTORY);
}



static void sRestoreVisualInventoryCosmetics(
    ActorCommonData & tACDPlayer)
{
    VisualCosmeticItem * tCurrentCosmeticItems = tACDPlayer.ptVisualInventory->tEquipmentCurrent.tCosmeticItems;

    if( tCurrentCosmeticItems[VCIS_WINGS].gbidItem != GBID_NONE )
    {
        if( !CosmeticItems_ValidateItem(tACDPlayer, tCurrentCosmeticItems[VCIS_WINGS].gbidItem, COSMETIC_ITEM_WINGS) )
        {
            VisualInventorySetCosmeticItemInSlot(tACDPlayer.id, VCIS_WINGS, GBID_NONE);
        }
    }

    if( tCurrentCosmeticItems[VCIS_PENNANT].gbidItem != GBID_NONE )
    {
        if( !CosmeticItems_ValidateItem(tACDPlayer, tCurrentCosmeticItems[VCIS_PENNANT].gbidItem, COSMETIC_ITEM_PENNANT) )
        {
            VisualInventorySetCosmeticItemInSlot(tACDPlayer.id, VCIS_PENNANT, GBID_NONE);
        }
    }

    if( tCurrentCosmeticItems[VCIS_PORTRAIT].gbidItem != GBID_NONE )
    {
        if( !CosmeticItems_ValidateItem(tACDPlayer, tCurrentCosmeticItems[VCIS_PORTRAIT].gbidItem, COSMETIC_ITEM_FRAME) )
        {
            VisualInventorySetCosmeticItemInSlot(tACDPlayer.id, VCIS_PORTRAIT, GBID_NONE);
        }
    }

    if( tCurrentCosmeticItems[VCIS_PET].gbidItem != GBID_NONE )
    {
        if( !CosmeticItems_ValidateItem(tACDPlayer, tCurrentCosmeticItems[VCIS_PET].gbidItem, COSMETIC_ITEM_PET) )
        {
            VisualInventorySetCosmeticItemInSlot(tACDPlayer.id, VCIS_PET, GBID_NONE);
        }
        else
        {
            Player *ptPlayer = PlayerGetByACD(tACDPlayer.id);
            if( ptPlayer )
            {
                SPlayerActivatePet(*ptPlayer, tCurrentCosmeticItems[VCIS_PET].gbidItem);
            }
        }
    }


    tACDPlayer.ModifiedMark(ACDMODIFIED_VISUAL_INVENTORY);
}

namespace SHero {

static void sSaveUsableItemSlot(
    const Player & tPlayer,
    D3::Hero::SavedData & tSavedData)
{
    const PlayerSavedData * ptPlayerData = &tPlayer.tHeroStateData.tSavedData;
    tSavedData.set_gbid_potion_button(ptPlayerData->tPotionBtn.m_gbidItem);

    tSavedData.clear_potion_button_item();
    if (ptPlayerData->tPotionBtn.m_annItem != ANN_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(ptPlayerData->tPotionBtn.m_annItem));
        if (ptACDItem)
        {
            const InventoryLocation * ptInvLoc = ptACDItem->TryToGetInventoryLocation();
            if (ptInvLoc && ptInvLoc->eSlot != 16)
            {
                D3::Hero::SavedItemLink * ptItemLoc = tSavedData.mutable_potion_button_item();
                ptItemLoc->set_x(ptInvLoc->pt.x);
                ptItemLoc->set_y(ptInvLoc->pt.y);
            }
        }
    }
}



static void sRestoreUsableItemSlot(
    Player & tPlayer,
    const D3::Hero::SavedData & tSavedData)
{
    PlayerSavedData * ptPlayerData = &tPlayer.tHeroStateData.tSavedData;

    if (tSavedData.has_potion_button_item())
    {
        const D3::Hero::SavedItemLink & tItemLoc = tSavedData.potion_button_item();

        InventoryLocation tInvLoc;
        tInvLoc.idACDContainer = tPlayer.idPlayerACD;
        tInvLoc.eSlot = SLOT_PLAYER_BACKPACK;
        tInvLoc.pt.Set(tItemLoc.x(), tItemLoc.y());

        ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);

        ActorCommonData * ptACDItem = ACDTryToGet(idACDItemInSlot);

        if (!ptACDItem || !ItemIsLegendary(ptACDItem->hGB.gbid))
        {

            tInvLoc.eSlot = SLOT_PLAYER_BASE_HEALTHPOTION;
            tInvLoc.pt.Set(0, 0);

            idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);


            ptACDItem = ACDTryToGet(idACDItemInSlot);
        }


        if (ptACDItem && ItemCanBeAssignedToUsableItemButton(*ptACDItem))
        {
            ptPlayerData->tPotionBtn.SetItemANN(ANNFromACD(idACDItemInSlot));
            return;
        }
    }

    else if(tSavedData.has_gbid_potion_button())
    {
        GBID gbidPotion = tSavedData.gbid_potion_button();

        if(gbidPotion != GBID_NONE)
        {
            ptPlayerData->tPotionBtn.SetItemGBID(gbidPotion);
            return;
        }
    }

    // No row of its own: the original's only PlayerAssignBaseHealthPotion call (+0x10f8) is at line 0, merged
    // with CreateActor's own call (B05, after GameIsPvP at 1379); every failure path of this helper branches to it (H04 evidence, sources).
    PlayerAssignBaseHealthPotion(&tPlayer);
}



static void sSaveLoadouts(
    const Player & tPlayer,
    D3::Hero::SavedDefinition & tSavedDefinition)
{
    D3::Hero::EquipmentManagerData & tSavedData = *tSavedDefinition.mutable_equipment_manager_data();
    tSavedData.clear_loadouts();


    for (int32 nLoadout = 0; nLoadout < 5; ++nLoadout)
    {
        D3::Hero::Loadout & tSavedLoadout = *tSavedData.add_loadouts();
        const LoadoutSavedData & tPlayerLoadout = tPlayer.tLoadouts[nLoadout];
        tPlayerLoadout.Export(tSavedLoadout, tPlayer.idPlayerACD);
    }
}
}

namespace SHero {

static void sRestoreLoadouts(
    Player & tPlayer,
    const D3::Hero::SavedDefinition & tSavedDefinition)
{
    if (!tSavedDefinition.has_equipment_manager_data())
    {
        return;
    }

    const D3::Hero::EquipmentManagerData & tSavedData = tSavedDefinition.equipment_manager_data();


    for (int32 nLoadout = 0; nLoadout < 5 && nLoadout < tSavedData.loadouts_size(); ++nLoadout)
    {
        const D3::Hero::Loadout & tSavedLoadout = tSavedData.loadouts(nLoadout);
        LoadoutSavedData & tPlayerLoadout = tPlayer.tLoadouts[nLoadout];
        tPlayerLoadout.Import(tSavedLoadout);
    }
}
}




static BOOL sFindEquipmentManagerItemCallBack(
    const InventoryLocation & tInvLoc,
    void * pCallBackData)
{
    if (!pCallBackData)
        return FALSE;



    PlayerRestoreLoadoutEquipment * ptCallbackData = static_cast<PlayerRestoreLoadoutEquipment *>(pCallBackData);

    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquareItem);
    if (!ptACDItem)
        return TRUE;




    uint64 uFakeItemId = ItemGenerateConsoleEquipmentManagerID(*ptACDItem);








    for (int32 nLoadout = 0; nLoadout < 5; ++nLoadout)
    {
        LoadoutSavedData & tPlayerLoadout = ptCallbackData->ptLoadouts[nLoadout];
        if (tPlayerLoadout.bEmpty)
            continue;



        for (int32 i = 0; i < 13; ++i)
        {
            LoadoutItemData & tLoadoutItem = tPlayerLoadout.tEquipment[i];
            if (uFakeItemId != tLoadoutItem.uOriginalItemIdLow)
                continue;





            InventorySlotType eInvSlot = static_cast<InventorySlotType>(i + 1);

            InventoryLocation tInvLoc;
            ACDInventoryLocationSet(tInvLoc, tACDContainer.id, eInvSlot, 0, 0);
            tLoadoutItem.annItem = ANNFromACD(ptACDItem->id);
            break;
        }



        LoadoutPotionData & tLoadoutPotion = tPlayerLoadout.tEquippedPotion;
        if (uFakeItemId == tLoadoutPotion.uOriginalItemIdLow)
        {
            tLoadoutPotion.annPotion = ANNFromACD(ptACDItem->id);
            tLoadoutPotion.gbidPotion = ptACDItem->hGB.gbid;
        }
    }
    return TRUE;
}

namespace SHero {
void SetHandicapValues(ActorCommonData & tACD);  // SHero.cpp:2615 (DWARF); called at 1158, above its definition
static void sGrantDefaultInventory(Player & tPlayer, GBID gbid);  // SHero.cpp:1515 (DWARF, _ZN5SHeroL...); called at 1193, above its definition



static void sRestoreLoadoutEquipment(
    Player & tPlayer,
    const D3::Hero::SavedDefinition & tSavedDefinition)
{
    ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    // No row of its own: the null test is ACDTryToGet's own exit branch (+0x14e4..+0x1554 -> +0x1a5c, H05 evidence).
    if (!ptACDPlayer)
    {
        return;
    }
    if (!tSavedDefinition.has_equipment_manager_data())
    {
        return;
    }

    PlayerRestoreLoadoutEquipment tData;
    tData.ptLoadouts = tPlayer.tLoadouts;

    PlayerInventoryIterateLoadoutEquipmentSlots(*ptACDPlayer, sFindEquipmentManagerItemCallBack, &tData);

    for (int32 nLoadout = 0; nLoadout < 5; ++nLoadout)
    {
        LoadoutSavedData & tPlayerLoadout = tPlayer.tLoadouts[nLoadout];
        for (int32 i = 0; i < 13; ++i)
        {
            LoadoutItemData & tLoadoutItem = tPlayerLoadout.tEquipment[i];

            if (tLoadoutItem.uOriginalItemIdLow != OnlineService::ItemId::INVALID.GetLow() &&
                tLoadoutItem.annItem == ANN_INVALID)
            {
                tLoadoutItem.annItem = (ACDNetworkName)8640;  // literal 8640 (0x21c0, mov w25 at +0x1614): no enumerator or constant of this value in any CU's DWARF
            }
        }

        LoadoutPotionData & tLoadoutPotion = tPlayerLoadout.tEquippedPotion;
        if (tLoadoutPotion.uOriginalItemIdLow != OnlineService::ItemId::INVALID.GetLow() &&
            tLoadoutPotion.annPotion == ANN_INVALID)
        {

            InventoryLocation tInvLoc;
            ACDInventoryLocationSet(tInvLoc, tPlayer.idPlayerACD, SLOT_PLAYER_BASE_HEALTHPOTION, 0, 0);
            ACDID idPotion = ACDInventoryGetItemInSlot(tInvLoc);
            ActorCommonData * ptACDDefaultPotion = ACDTryToGet(idPotion);
            // No row of its own: the null test is ACDTryToGet's own exit branch (+0x1998..+0x1a08 -> +0x1624, line 893).
            if (ptACDDefaultPotion)
            {
                uint64 uFakeItemId = ItemGenerateConsoleEquipmentManagerID(*ptACDDefaultPotion);








                if (uFakeItemId == 0 || tLoadoutPotion.uOriginalItemIdLow == uFakeItemId)
                {
                    tLoadoutPotion.annPotion = ANNFromACD(idPotion);
                }
            }

            if (tLoadoutPotion.annPotion == ANN_INVALID)
            {
                tLoadoutPotion.annPotion = (ACDNetworkName)8640;  // same literal as line 862 (str w25 at +0x1664)


                ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
                if (ptACDPlayer)
                {
                    ActorCommonData *ptACDNewPotion = NULL;
                    InventoryLocation tInvLoc;
                    if (SPlayerFindMatchingPotion(*ptACDPlayer, tLoadoutPotion.gbidPotion, ptACDNewPotion, tInvLoc))
                    {
                        tLoadoutPotion.annPotion = ANNFromACD(ptACDNewPotion->id);
                    }
                }
            }
        }
    }
}



}  // namespace SHero (sFixupStashEntitlements at 916 is file scope in the DWARF)















namespace SHero {

ActorID CreateActor(
    uint32 dwPlayerFlags,
    const WorldPlace & tPlace,
    const Quaternion & q,
    Player & tPlayer,
    const D3::Hero::SavedDefinition & tHeroDefinition,
    const D3::Account::SavedDefinition & tAccountDefinition,
    const D3::Items::Mails & tMails,
    const uint32 )
{
    BOOL bNewHero;





    GBID gbidHeroClass = GBID_NONE;
    if (tHeroDefinition.has_digest())
    {
        gbidHeroClass = tHeroDefinition.digest().gbid_class();
    }

    if (tHeroDefinition.has_saved_attributes() &&
        tHeroDefinition.saved_attributes().gb_handle().game_balance_type() != -1)
    {
        gbidHeroClass = tHeroDefinition.saved_attributes().gb_handle().gbid();
    }

    if (!tHeroDefinition.has_digest()
        || tHeroDefinition.saved_attributes().gb_handle().game_balance_type() == -1)
    {
        // No row of its own: the TRUE store is the line-0 `orr w20, wzr, #0x1` at +0x13c that both conditions branch to.
        bNewHero = TRUE;
    }
    else
    {


        dwPlayerFlags = tHeroDefinition.digest().player_flags();



        bNewHero = (tHeroDefinition.saved_attributes().attributes().size() == 0);
    }

    if (GameTypeIsPvP())
    {
        if (GameIsParamFlagSet(GPF_FORCE_NEW_HEROS))
        {
            // No row of its own: merged into the line-0 `orr w8, w0, w20` (+0x154) feeding the bNewHero test at 993.
            bNewHero = TRUE;
        }
    }







	if (bNewHero)
	{

		if (!GBIDExists(gbidHeroClass, GB_HEROS))
		{
			gbidHeroClass = GlobalGBIDGet(GB_HEROS);
			if (!cgfDefaultPlayerSexIsMale) dwPlayerFlags |= (1u << PLAYERFLAG_FEMALE); else dwPlayerFlags &= ~(1u << PLAYERFLAG_FEMALE);  // Inference: negated test with |= first, from the and/orr order and csel ne at 999 (all col 4).
		}
	}



	ActorAddStruct tActorAdd;
	tActorAdd.hGB.eType = GB_HEROS;
	tActorAdd.hGB.gbid = gbidHeroClass;
	tActorAdd.snoActor = GameBalanceGetHeroSNO(tActorAdd.hGB, dwPlayerFlags);
	tActorAdd.dwActorAddFlags = (1 << ACTORADD_FAST_MOVER);
    tActorAdd.ptPlayer = &tPlayer;
	tActorAdd.eFastAttribType = FAT_COMPLEX_ACTOR;
	tActorAdd.eTeam = tPlayer.eTeam;
	tActorAdd.tPlace = tPlace;
	tActorAdd.qOrientation = q;

	ActorID idActor = ActorAdd(tActorAdd);

	ACDID idACD = ActorGet(idActor)->idACD;

	ScopedGlobalDefinitionLockR pdefGlobal;


	ActorCommonData & tACD = ACDGet(idACD);

	// Bare scope: the DWARF block holding pdefHeros..flMovementScalarCap (lines 1028-1191) ends before the !bNewHero branch at 1198.
	{
		{
			ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
			const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;


			tACD.AttributesSetInt(ATTRIBUTE_LEVEL, 1);
			tACD.SetExperienceNext(tExpTable.ptExperienceLevels[0].nDeltaXP);
			tACD.SetExperienceGranted(tExpTable.ptExperienceLevels[0].nPlayerXPValue);
			tACD.AttributesSetInt(ATTRIBUTE_PVP_RANK, 1);
			tACD.SetPvPExperienceNext(tExpTable.ptExperienceLevels[1].nPVPXPToLevel);
		}
		{
			ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT));
			const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;

			tACD.AttributesSetInt(ATTRIBUTE_ALT_LEVEL, 0);
			tACD.SetAltExperienceNext(tExpTableAlt.ptExperienceLevelsAlt[0].nDeltaXP);
		}

		const GameBalanceDefinition * pdefHeros;
		GBHandle hGB(GB_HEROS, gbidHeroClass);
		const HeroData * pHero = (const HeroData *)GBLockR(hGB, pdefHeros, TRUE);


		tACD.AttributesSetFloat(ATTRIBUTE_ARMOR, pHero->flAttributesArmor);
		tACD.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, pHero->flAttributesHitPoints);

		tACD.AttributesSetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE, pHero->ePrimaryDamageAttribute);

		tACD.AttributesSetFloat(ATTRIBUTE_CORE_ATTRIBUTES_FROM_ITEM_BONUS_MULTIPLIER, 1.0f);

		tACD.AttributesSetFloat(ATTRIBUTE_STRENGTH, pHero->flAttributesStrength);
		tACD.AttributesSetFloat(ATTRIBUTE_DEXTERITY, pHero->flAttributesDexterity);
		tACD.AttributesSetFloat(ATTRIBUTE_INTELLIGENCE, pHero->flAttributesIntelligence);
		tACD.AttributesSetFloat(ATTRIBUTE_VITALITY, pHero->flAttributesVitality);

		tACD.AttributesSetFloat(ATTRIBUTE_HITPOINTS_FACTOR_LEVEL, pHero->flAttributesHitPointsFactorLevel);


		tACD.AttributesSetFloat(ATTRIBUTE_HITPOINTS_CUR, tACD.GetHitPointsMaxTotal());



		ResourceType ePrimaryResource = pHero->ePrimaryResourceType;
		tACD.AttributesSetInt(ATTRIBUTE_RESOURCE_TYPE_PRIMARY, ePrimaryResource);

		if (ePrimaryResource != RESOURCE_TYPE_INVALID)
		{
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_MAX, ePrimaryResource),
				pHero->flAttributesPrimaryResourceBase);
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_FACTOR_LEVEL, ePrimaryResource),
				pHero->flAttributesPrimaryResourceFactorLevel);
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, ePrimaryResource),
				pHero->flAttributesPrimaryResourceRegen);
		}



		ResourceType eSecondaryResource = pHero->eSecondaryResourceType;
		tACD.AttributesSetInt(ATTRIBUTE_RESOURCE_TYPE_SECONDARY, eSecondaryResource);

		if (eSecondaryResource != RESOURCE_TYPE_INVALID)
		{
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_MAX, eSecondaryResource),
				pHero->flAttributesSecondaryResourceBase);
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_FACTOR_LEVEL, eSecondaryResource),
				pHero->flAttributesSecondaryResourceFactorLevel);
			tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, eSecondaryResource),
				pHero->flAttributesSecondaryResourceRegen);
		}

		tACD.AttributesSetFloat(ATTRIBUTE_WALKING_RATE, pHero->flAttributesWalkSpeed);
		tACD.AttributesSetFloat(ATTRIBUTE_RUNNING_RATE, pHero->flAttributesRunSpeed);
		tACD.AttributesSetFloat(ATTRIBUTE_SPRINTING_RATE, pHero->flAttributesSprintSpeed);
		tACD.AttributesSetFloat(ATTRIBUTE_PROJECTILE_SPEED, pHero->flAttributesProjSpeed);

		tACD.AttributesSetFloat(ATTRIBUTE_KNOCKBACK_WEIGHT, pHero->flAttributesKnockbackWeight);

		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_PHYSICAL), pHero->flAttributesResistPhysical);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_FIRE), pHero->flAttributesResistFire);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_LIGHTNING), pHero->flAttributesResistLightning);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_COLD), pHero->flAttributesResistCold);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_POISON), pHero->flAttributesResistPoison);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_ARCANE), pHero->flAttributesResistArcane);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_HOLY), pHero->flAttributesResistArcane);
        tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_RESISTANCE), pHero->flAttributesResistChill);
		tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, ATTRIBUTE_STUNNED), pHero->flAttributesResistStun);

		tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR, 1.0f);
		tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, 0.0f);
		tACD.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND, 1.0f);
		tACD.AttributesSetFloat(ATTRIBUTE_CASTING_SPEED, 1.0f);

		float flAttackSpeedCap = pdefGlobal->ptServerData->flAttackSpeedItemPercentCap;
		tACD.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_CAP, flAttackSpeedCap / 100.0f);

		float flCooldownMinTime = pdefGlobal->ptServerData->flCooldownMinimumTime;
		tACD.AttributesSetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME, flCooldownMinTime);
		// Inference: operand order (1.0f first) from the fmax (not fmaxnm) at 1126, as ConsoleTargeting 712 / RActorGfx.
		float flMovementScalarCap = Math::Max(1.0f, pdefGlobal->ptServerData->flMovementScalarCap);
		tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR_CAP, flMovementScalarCap);

		tACD.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BASE, pHero->flAttributesCritPercentBase);
		tACD.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, 0.0f);
		tACD.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_UNCAPPED, 0.0f);
		tACD.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_CAP, pHero->flAttributesCritPercentCap);
		tACD.AttributesSetFloat(ATTRIBUTE_CRIT_DAMAGE_CAP, pHero->flAttributesCritDamageCap);


		tACD.AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS, 0.0f);

		tACD.AttributesSetFloat(ATTRIBUTE_GETHIT_MAX_BASE, pHero->flAttributesGetHitMaxBase);
		tACD.AttributesSetFloat(ATTRIBUTE_GETHIT_MAX_PER_LEVEL, pHero->flAttributesGetHitMaxPerLevel);
		tACD.AttributesSetFloat(ATTRIBUTE_GETHIT_RECOVERY_BASE, pHero->flAttributesGetHitRecoveryBase);
		tACD.AttributesSetFloat(ATTRIBUTE_GETHIT_RECOVERY_PER_LEVEL, pHero->flAttributesGetHitRecoveryPerLevel);

		tACD.AttributesSetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, pHero->flAttributesHPRegen);
		tACD.AttributesSetFloat(ATTRIBUTE_OOC_HEALTH_REGEN_PERCENT, pHero->flAttributesOOCHealthRegenPercent);
		tACD.AttributesSetFloat(ATTRIBUTE_OOC_MANA_REGEN_PERCENT, pHero->flAttributesOOCManaRegenPercent);

		tACD.AttributesSetFloat(ATTRIBUTE_CLASS_DAMAGE_REDUCTION_PERCENT, pHero->flAttributesClassDamageReductionPercent);
		tACD.AttributesSetFloat(ATTRIBUTE_CLASS_DAMAGE_REDUCTION_PERCENT_PVP, pHero->flAttributesClassDamageReductionPercentPVP);

		tACD.AttributesSetInt(ATTRIBUTE_POTION_DILUTION_DURATION, (int32)pHero->flAttributesPotionDilutionDuration);
		tACD.AttributesSetFloat(ATTRIBUTE_POTION_DILUTION_SCALAR, pHero->flAttributesPotionDilutionScalar);

		tACD.AttributesSetFloat(ATTRIBUTE_DUAL_WIELD_BOTH_ATTACK_CHANCE, pHero->flAttributesDualWieldBothAttackChance);

		tACD.AttributesSetFloat(ATTRIBUTE_FREEZE_CAPACITY, pHero->flAttributesFreezeCapacity);
		tACD.AttributesSetFloat(ATTRIBUTE_THAW_RATE, pHero->flAttributesThawRate);

		SetHandicapValues(tACD);

		tACD.AttributesSetInt(ATTRIBUTE_BACKPACK_SLOTS, 60);
		tACD.AttributesSetInt(ATTRIBUTE_SHARED_STASH_SLOTS, 20);
		tACD.AttributesSetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER, 0);
		tACD.SetStoneOfRecallUnlocked(FALSE);

		tACD.AttributesSetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceBase);
		tACD.AttributesSetInt(ATTRIBUTE_TARGETED_RARE_CHANCE, pdefGlobal->ptServerData->nTargetedRareDropChanceBase);
		tACD.AttributesSetInt(ATTRIBUTE_TARGETED_MAGIC_CHANCE, pdefGlobal->ptServerData->nTargetedMagicDropChanceBase);

		tACD.AttributesSetInt(ATTRIBUTE_DAMAGE_DONE_TRACKING_START_TICK, GameGetServerTime());

		tACD.AttributesSetInt(ATTRIBUTE_CORPSE_RESURRECTION_CHARGES, pdefGlobal->nCorpseResurrectionMaxCharges);

		tACD.AttributesSetFloat(ATTRIBUTE_REST_EXPERIENCE_BONUS_PCT, pdefGlobal->flRestXPBonusPercent);

        HeroEnsureHeroHasAllSkillAttributes(tACD, bNewHero);

        if (bNewHero)
        {
            sSetupStartingSkillAndButton(tACD, tPlayer, pHero->snoStartingLMBSkill, 0);
            sSetupStartingSkillAndButton(tACD, tPlayer, pHero->snoStartingRMBSkill, 1);
        }

        GBUnlockR(pdefHeros);

        tPlayer.tHeroStateData.uPlayerFlags = dwPlayerFlags;
        tPlayer.uSeasonCreated = tHeroDefinition.digest().season_created();

        if (bNewHero)
        {
            // No row of its own here: the PvP test's call is at 1191 (cols 17/18).
            if (!GameIsPvP())
            {
                sGrantDefaultInventory(tPlayer, gbidHeroClass);
            }
        }
	}
    // No row of its own: tested by the cbz at 1188 (+0xa1c), whose not-taken path continues at +0xa84 (1200).
    if (!bNewHero)
    {
        HeroDefinitionRestoreAttributes(tACD, tHeroDefinition.saved_attributes());
        sRecalculateBaseAttributes(idACD);

        sRestoreVisualInventory(tACD, tHeroDefinition.digest().visual_equipment());


        uint64 nXPNext = tACD.GetExperienceNext();
        int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);

        if (nLevel >= 1 && nLevel < 70)
        {
            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
            uint64 nXPToLevel = tExpTable.ptExperienceLevels[nLevel - 1].nDeltaXP;

            BOOL bNewX1PlayerAt60 = (tPlayer.bNewX1Player && nLevel == 60);
            if (nXPToLevel < nXPNext || bNewX1PlayerAt60)
            {


                tACD.SetExperienceNext(nXPToLevel);
            }
        }
    }

    tPlayer.bNewX1Player = FALSE;

    const GameBalanceDefinition * pdefGB;
    const HeroData * pHeroData = (const HeroData *)GBLockR(tACD.hGB, pdefGB, TRUE);
	if (pHeroData)
	{
		tACD.SetSkillKit(pHeroData->arSkillKit[0], FALSE);
		GBUnlockR(pdefGB);
	}

    SPlayerRestoreRunes(tACD, tPlayer);



    if (SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_LEVEL_70))
    {
        SPlayerSetLevelCap(tACD, OnlineService::CONTENT_LICENSE_LEVEL_70);
        tACD.AttributesSetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR, 0);  // no row of its own: the call at +0xc1c (line 0) is shared with 1247 and 1254
    }
    else if (SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_LEVEL_60))
    {
        SPlayerSetLevelCap(tACD, OnlineService::CONTENT_LICENSE_LEVEL_60);
        tACD.AttributesSetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR, 0);  // no row of its own: shared line-0 call at +0xc1c
    }
    else if (!GameTypeIsPvP())
    {
        tACD.AttributesSetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR, 1);
        int32 nTrialCap = pdefGlobal->nTrialVersionLevelCap;
        nTrialCap = Math::Min(nTrialCap, 70);
        tACD.AttributesSetInt(ATTRIBUTE_LEVEL_CAP, nTrialCap);
    }

    if (!GameIsStartUp())
    {
        ACDInventoryCreateBaseHealthPotion(tACD);
    }

    if (!bNewHero)
    {

        tACD.SetTeam(tPlayer.eTeam);


        tACD.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_MAINACTOR, -1);
        tACD.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_ACD, ACDID_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_ATTACHED_TO_ANN, ANN_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_ATTACHMENT_ANN, ANN_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_ROOT_TARGET_ACD, ACDID_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_WIZARD_SLOWTIME_PROXY_ACD, ACDID_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, ACDID_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_NPC_TALK_TARGET_ANN, ANN_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_NPC_CONV_TARGET_ANN, ANN_INVALID);
        tACD.AttributesSetInt(ATTRIBUTE_LOOK_TARGET_SERVER_ANN, ANN_INVALID);


		HeroEnsureHeroHasAllSkillAttributes(tACD, bNewHero);

		if (!GameIsStartUp())
		{



			ACDInventorySavedStateRestore(tACD, tHeroDefinition.items().items());


			if (!GameActiveGameCommonDataIsClient())
			{




                if (tPlayer.tHirelingStable.eActiveHireling != 0 && !GameTypeIsPvP())
                {

                    ACDID idACDHireling = SHirelingLoadFromStable(idACD, tPlayer.tHirelingStable.eActiveHireling);

                    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
                    // No row of its own: the null test is ACDTryToGet's own exit branch (+0x1c64).
                    if (ptACDHireling)
                    {
                        SNO snoConversation = GlobalSNOGet(GLOBALSNO_CONVERSATION_HIRELING_JOIN_PLAYER);
                        AISetCalloutCooldown(*ptACDHireling, snoConversation, 10.0f, 0.0f);
                    }
                }
                else
                {
                    SGameStatsPlayerTimedEventStart(&tPlayer, STAT_COUNTER_HIRELING_UPTIME, 0, 0, TRUE);
                }






                SGame & tSGame = SGameManager::GameGet();
                if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
                {
                    SACDInventoryRemoveIllegalItems(tACD);
                }
			}
		}
    }



    const D3::Account::AccountPartition & tPartition = tAccountDefinition.partitions(0);

    // Inference (as Load, 1659-1661): bIsHardcore's initializer leaves no code (local optimised out, GetPartition inlined); only the GameIsSeasonal call survives.
    BOOL bIsHardcore = (tPlayer.tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE)) != 0;
    int32 uPartitionNum = GetPartition(bIsHardcore, GameIsSeasonal());




    D3::AttributeSerializer::SavedAttributes attributes;
    attributes.MergeFrom(tPartition.saved_attributes());
    attributes.MergeFrom(tAccountDefinition.saved_attributes());


    AccountDefinition_RestoreAttributes(tACD, attributes);

    sRestoreVisualInventoryCosmetics(tACD);





















    ExperienceUpdatePerLevelHitpointFactorVitality(tACD);


    ACDInventorySavedStateRestore(tACD, tPartition.items().items());


    SPlayerFixUpCurrenciesDiscovered(tACD);


    if (bNewHero)
    {
        if (!GameIsPvP())
        {
            // No row of its own: the call (+0x10f8) is at line 0, tail-merged with sRestoreUsableItemSlot's (H04, 711).
            PlayerAssignBaseHealthPotion(&tPlayer);
        }
    }
    else
    {
        sRestoreUsableItemSlot(tPlayer, tHeroDefinition.saved_data());
    }


    if (tAccountDefinition.has_digest())
    {
        tPlayer.tAccount.tBanner.Import(tAccountDefinition.digest().banner_configuration());
        tPlayer.tAccount.uDigestFlags = tAccountDefinition.digest().flags();
    }

    tPlayer.uAcceptedLicenseBitsAccount = tPartition.accepted_license_bits();
    tPlayer.uAcceptedLicenseBitsHero = tHeroDefinition.accepted_license_bits();
    tPlayer.uGuildId = tAccountDefinition.digest().guild_id();





    if (!tPlayer.bIsGuestPlayerServerOnly)
    {
        uint32 uConsoleLicenses = SPlayerGetConsoleLicenseFlags(tPlayer);
        ConsoleUpdateBonusItems(tPlayer, tACD, uConsoleLicenses);
    }





    float flHitChance = pdefGlobal->ptServerData->flHitChance;
    tACD.AttributesSetFloat(ATTRIBUTE_HIT_PERCENT, flHitChance);
    tACD.AttributesSetInt(ATTRIBUTE_BUSY, 0);

    ActorPowersGrantAllGlobalPowers(tACD);
    SPlayerInitializeParagonPoints(tPlayer);


	HeroCheckAreaAfterCreate(idActor);
	ACDInventoryEquipmentOrSkillChanged(tACD);
    ACDInventoryEquippedItemsChanged(tACD);
    ACDInventoryUpdateSkillButtons(tACD);


    SPlayerEvaluateMultiplayerBuff();


    SGameSendDungeonFinderSeedToPlayer(tPlayer);
    SGameSendDungeonFinderParticipatingPlayerCountToPlayer(tPlayer);
    SGameSendDungeonFinderProgressToPlayer(tPlayer);
    SGameSendDungeonFinderTimedEventToPlayer(tPlayer);


    if (tACD.AttributesGetInt(ATTRIBUTE_ATTACHED_TO_ANN) != ANN_INVALID)
    {
        tACD.AttributesSetInt(ATTRIBUTE_ATTACHED_TO_ANN, ANN_INVALID);
    }

    int32 nHighestCompletedSoloRiftLevel = tACD.AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
    int32 nHighestUnlockedRiftLevel = tACD.AttributesGetInt(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL);
    if (nHighestCompletedSoloRiftLevel > 0 && nHighestUnlockedRiftLevel < nHighestCompletedSoloRiftLevel)
    {
        tACD.AttributesSetInt(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL, nHighestCompletedSoloRiftLevel);
    }

   if (nHighestCompletedSoloRiftLevel > 0 && tACD.AttributesGetInt(ATTRIBUTE_LAST_RIFT_STARTED_LEVEL) <= 0)
    {
        tACD.AttributesSetInt(ATTRIBUTE_LAST_RIFT_STARTED_LEVEL, nHighestCompletedSoloRiftLevel);
    }

	ActorSetBrain(idActor, ACTORBRAIN_PLAYER, NULL);
    tACD.SetCombatStatus(FALSE);

    if (SPlayerHasTradeRestrictions(tPlayer))
    {
        tACD.AttributesSetInt(ATTRIBUTE_ACCOUNT_TRADING_RESTRICTED, 1);
    }

    ACDInventoryRestoreMail(tACD, tMails);

    if (NephalemRift::IsTieredRiftActive() && NephalemRiftManager::PlayerPaidForRift(&tPlayer))
    {
        tACD.AttributesSetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN, 1);

        NephalemRiftManager::GreaterRiftPlayerState * ptGreaterRiftPlayerState = NephalemRiftManager::GetGreaterRiftPlayerState(&tPlayer);
        if (ptGreaterRiftPlayerState)
        {
            tACD.AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT, ptGreaterRiftPlayerState->nDeathCount);
        }
    }


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
        NephalemRiftManager::IsTieredRiftActive())
    {
        NephalemRiftManager::PlayerSetPaidForRift(&tPlayer, TRUE, TRUE, FALSE);

        if (GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead)
        {
            BlizzconEndScreenMessage tMessage;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, tPlayer);
            tMessage.ann = ANNFromACD(tACD.id);
            tMessage.nRewardTier = 0;  // no row of its own: merged with 1487's store into one stp (+0x13bc)
            MessageSendToClient(listDestPlayers, SMGS_BLIZZCON_RIFT_ENDED, &tMessage, sizeof(tMessage));
        }
    }












    sRestoreLoadoutEquipment(tPlayer, tHeroDefinition);


    tACD.ApplyAllPassives(FALSE);
    SPlayerRestoreCooldowns(tACD, tPlayer);

    return idActor;
}



static void sGrantDefaultInventory(
    Player & tPlayer,
    GBID gbid)
{
    GBHandle hGB(GB_HEROS, gbid);
    const GameBalanceDefinition * pdefGB;
    const HeroData * pCharData = (const HeroData*)GBLockR(hGB, pdefGB, TRUE);
    if (!pCharData)
        return;




    uint32 dwOldFlags = tPlayer.dwServerSidePrefFlags;
    tPlayer.dwServerSidePrefFlags |= (1 << SSPREF_AUTOEQUIP_ITEMS);

    if (pCharData->snoInventory != SNO_NONE)
    {
        SACDInventoryLoad(tPlayer.idPlayerACD, pCharData->snoInventory);
    }

    tPlayer.dwServerSidePrefFlags = dwOldFlags;

    GBUnlockR(pdefGB);
}












static void sVersionHero(
    const D3::Hero::SavedDefinition & tHeroDefinition,
    Player & tPlayer)
{
    uint32 uVersion = tHeroDefinition.version();
    if (uVersion != gptGroupDefHero->GetCurrentVersion(geSigmaPlatform))
    {
        if (uVersion < HERO_DEFINITION_VERSION_1_0_5)
        {

            for (int32 i = 0; i < 6; ++i)
            {
                tPlayer.tHeroStateData.uBossKillFlags[i] = 0;
            }


            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_SKELETON_KING)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_SKELETON_KING;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_WRETCHED_QUEEN)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_WRETCHED_QUEEN;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_BUTCHER)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_BUTCHER;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_BELIAL)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_BELIAL;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_AZMODAN)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_AZMODAN;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_DIABLO)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_DIABLO;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_SPIDER_QUEEN)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_SPIDER_QUEEN;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_MAGHDA)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_MAGHDA;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_ZOLTAN_KULLE)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_ZOLTAN_KULLE;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_GLUTTONY)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_GLUTTONY;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_SIEGEBREAKER_DEMON)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_SIEGEBREAKER_DEMON;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_MISTRESS_OF_PAIN)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_MISTRESS_OF_PAIN;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_DESPAIR)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_DESPAIR;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_IZUAL)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_IZUAL;
            if (tPlayer.tHeroStateData.uPlayerFlags & (1u << DEPRECATED_PLAYERFLAG_KILLED_UNIQUE_UNBURIED)) tPlayer.tHeroStateData.uBossKillFlags[0] |= 1u << BOSSKILLFLAG_UNIQUE_UNBURIED;


            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_SKELETON_KING);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_WRETCHED_QUEEN);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_BUTCHER);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_BELIAL);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_AZMODAN);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_DIABLO);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_SPIDER_QUEEN);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_MAGHDA);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_ZOLTAN_KULLE);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_GLUTTONY);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_SIEGEBREAKER_DEMON);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_MISTRESS_OF_PAIN);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_DESPAIR);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_IZUAL);
            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << DEPRECATED_PLAYERFLAG_KILLED_UNIQUE_UNBURIED);


            tPlayer.tHeroStateData.nNumQuestRewardEntries = 0;
            for (int32 i = 0; i < 100; ++i)
            {
                PlayerQuestRewardHistoryEntryInitialize(tPlayer.tHeroStateData.tQuestRewardHistory[i]);
            }


            for (int32 i = 0; i < tHeroDefinition.quest_reward_history_size(); ++i)
            {
                SPlayerReceivedQuestStepReward(tPlayer,
                    tHeroDefinition.quest_reward_history(i).sno_quest(),
                    tHeroDefinition.quest_reward_history(i).step_uid(),
                    tHeroDefinition.quest_reward_history(i).difficulty_deprecated());
            }
            uVersion = HERO_DEFINITION_VERSION_1_0_5;
        }


        if (uVersion < HERO_DEFINITION_VERSION_2_0_0)
        {
            tPlayer.tHeroStateData.tSavedData.uEventFlags = 0;

            for (int32 i = 0; i < 4; ++i)
            {
                tPlayer.tHeroStateData.tSavedData.tHirelingSavedData.tHireling[i].arnPowerKeyParam[4] = -1;
            }

            tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << PLAYERFLAG_RECEIVED_LOOT_FIND_BOOST);
            uVersion = HERO_DEFINITION_VERSION_2_0_0;
            tPlayer.bNewX1Player = true;
        }

        if (uVersion != gptGroupDefHero->GetCurrentVersion(geSigmaPlatform))
        {
            DisplayInternalError(0, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}



// NON_MATCHING: 3064 vs 3092 bytes; the new-hero test now matches (1650 written as if(flag) bNewHero = TRUE: the original ORs at 1666 and keeps bNewHero in w23); the quest history/reward/saved-quest loops (1724-1737) differ in induction-variable addressing and compare placement, and later blocks differ
void Load(
    Player & tPlayer,
    const D3::Hero::SavedDefinition & tHeroDefinition,
    const D3::Account::SavedDefinition & tAccountDefinition)
{
    BOOL bNewHero = FALSE;

    if (!tHeroDefinition.has_digest() ||
        !tHeroDefinition.has_saved_data() ||
        tHeroDefinition.saved_attributes().gb_handle().game_balance_type() == -1 ||
        tHeroDefinition.saved_attributes().attributes_size() == 0)
    {
        bNewHero = TRUE;
    }

    if (GameIsParamFlagSet(GPF_FORCE_NEW_HEROS))
    {
        bNewHero = TRUE;
    }



    const D3::Account::AccountPartition & tPartition = tAccountDefinition.partitions(0);

    // Inference: bIsHardcore initializer is unproven after optimization; current-player flags are assumed.
    BOOL bIsHardcore = (tPlayer.tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE)) != 0;
    int32 uPartitionNum = GetPartition(bIsHardcore, GameIsSeasonal());




    if (bNewHero)
    {
        HeroResetSavedData(tPlayer);
    }
    else
    {

        const D3::Hero::Digest & tHeroDigest = tHeroDefinition.digest();
        tPlayer.tHeroStateData.eLastPlayedAct = tHeroDigest.last_played_act();
        tPlayer.tHeroStateData.eHighestUnlockedAct = tHeroDigest.highest_unlocked_act();
        tPlayer.tHeroStateData.uPlayerFlags = tHeroDigest.player_flags();
        tPlayer.tHeroStateData.uPlayerFlags2 = tHeroDigest.player_flags2();
        tPlayer.tHeroStateData.tSavedData.Import(tHeroDefinition.saved_data());



        for (int32 i = 0; i < 6; ++i)
        {
            SNO snoSkillCurr = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
            BOOL bClearSkillSlot = snoSkillCurr != SNO_NONE && !SNOFileExists(POWER_SNO, snoSkillCurr);
            if (bClearSkillSlot)
            {
                tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill = SNO_NONE;
                tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].eRuneType = -1;
                tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].dwLastLifeOnHitTick = 0;
                tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].idChargesTimer = -1;
                tPlayer.tHeroStateData.tSavedData.tHotbarBtnAssignments[i].Empty();
            }
        }

        for (int32 i = 0; i < 4; ++i)
        {
            SNO snoTraitCurr = tPlayer.tHeroStateData.tSavedData.snoTraits[i];
            if (snoTraitCurr != SNO_NONE && !SNOFileExists(POWER_SNO, snoTraitCurr))
            {
                tPlayer.tHeroStateData.tSavedData.snoTraits[i] = SNO_NONE;
            }
        }


        for (int32 i = 0; i < 3; ++i)
        {
            GBID gbidItem = tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[i];
            if (gbidItem != -1 && !GBIDExists(gbidItem, GB_ITEMS))
            {
                tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[i] = -1;
            }
        }




        tPlayer.tPlayerResumeStateUponJoining.eLastPlayedAct = tHeroDigest.last_played_act();
        tPlayer.tPlayerResumeStateUponJoining.snoLastPlayedQuest = tHeroDigest.last_played_quest();
        tPlayer.tPlayerResumeStateUponJoining.nLastPlayedQuestStep = tHeroDigest.last_played_quest_step();
        tPlayer.tPlayerResumeStateUponJoining.uActivatedWaypoints = tPlayer.tHeroStateData.tSavedData.m_ActivatedWaypoints;
        tPlayer.tPlayerResumeStateUponJoining.tLastSavePoint = tPlayer.tHeroStateData.tSavedData.tLastSavePoint;
        tPlayer.tPlayerResumeStateUponJoining.nHandicapLevelSnapshot = tHeroDefinition.saved_data().main_quest_handicap_snapshot();

        for (int32 i = 0; i < 200 && i < tHeroDigest.quest_history_size(); ++i)
        {
            tPlayer.tHeroStateData.tQuestHistory[i].Import(tHeroDigest.quest_history(i));
        }

        tPlayer.tHeroStateData.nNumQuestRewardEntries = tHeroDefinition.quest_reward_history_size();
        for (int32 i = 0; i < 100 && i < tHeroDefinition.quest_reward_history_size(); ++i)
        {
            tPlayer.tHeroStateData.tQuestRewardHistory[i].Import(tHeroDefinition.quest_reward_history(i));
        }

        for (int32 i = 0; i < 64 && i < tHeroDefinition.saved_quest_size(); ++i)
        {
            tPlayer.tHeroStateData.tSavedQuest[i].Import(tHeroDefinition.saved_quest(i));
        }

        for (int32 i = 0; i < 6 && i < tHeroDefinition.saved_data().boss_kill_flags_size(); ++i)
        {
            tPlayer.tHeroStateData.uBossKillFlags[i] = tHeroDefinition.saved_data().boss_kill_flags(i);
        }

        if (tPartition.stash_icons().size())
        {
            size_t nAmountToCopy = Math::Min(tPartition.stash_icons().size(), sizeof(tPlayer.bStashIcons));
            SigmaMemoryCopy(tPlayer.bStashIcons, tPartition.stash_icons().c_str(), nAmountToCopy);
        }


        for (const D3::Hero::ConsoleRandomVendorSeed & tVendorSeed : tHeroDefinition.console_data().random_item_vendor_seed_list().console_random_vendor_seed())
        {
            tPlayer.tMapVendorSeeds[tVendorSeed.item_slot_key()] = blz::pair<uint32, uint32>(tVendorSeed.random_item_seed(), tVendorSeed.random_item_carry());
        }

        for (const D3::Account::ConsoleRandomTransmuteSeed & tCubeSeed : tPartition.console_partition_data().console_random_transmute_seed_list().console_random_transmute_seed())
        {
            tPlayer.tMapPartitionWideCubeSeeds[tCubeSeed.item_slot_key()] = blz::pair<uint32, uint32>(tCubeSeed.random_item_seed(), tCubeSeed.random_item_carry());
        }

        tPlayer.uCubeBaseSeed = tAccountDefinition.console_data().console_cube_base_seed();

        for (const D3::Account::ConsoleChallengeRiftReward & tReward : tAccountDefinition.console_data().challenge_rift_reward())
        {

            if (!GameIsSeasonal() || tReward.season_earned() == OnlineService::GetSeasonNum())
            {
                tPlayer.tChallengeRiftRewards.push_back(tReward);
            }
        }



        SItemCrafting_InitSavedData(tPlayer);

        if (!GameIsStartUp())
        {
            QuestsVersionForPlayer(tPlayer.tHeroStateData);


            SHirelingRestoreStableInfo(tPlayer);


            ConversationMgr::MarkSavedConversationsAsHeard(tPlayer);


            sVersionHero(tHeroDefinition, tPlayer);
        }

        sRestoreLoadouts(tPlayer, tHeroDefinition);
    }


    tPlayer.tCrafterSavedData.Import(tPartition.crafter_data(), &tAccountDefinition);
    tPlayer.tCosmeticItemSavedData.Import(&tAccountDefinition);


    if (tAccountDefinition.has_delivered_rewards())
    {
        SPlayerImportDeliveredRewards(tPlayer, tAccountDefinition.delivered_rewards());
    }
    if (tPartition.has_currency_data())
    {

        tPlayer.tCurrencySavedData.Import(tPartition.currency_data(), TRUE);
    }
    if (tAccountDefinition.has_account_wide_currency_data())
    {

        tPlayer.tCurrencySavedData.Import(tAccountDefinition.account_wide_currency_data(), FALSE);
    }

    SPlayerConsumables & tConsumables = SPlayerGetConsumables(tPlayer);

    tConsumables.Import(tAccountDefinition.consumables(), TRUE);


    if (tAccountDefinition.seen_tutorials().size())
    {
        SigmaMemoryCopy(tPlayer.tSeenTutorials.GetRawBuffer(), tAccountDefinition.seen_tutorials().c_str(), tAccountDefinition.seen_tutorials().size());
    }

    if (tAccountDefinition.has_num_vote_kicks_participated_in())
    {
        tPlayer.nKicksParticipatedIn = tAccountDefinition.num_vote_kicks_participated_in();
    }

    if (tAccountDefinition.has_num_public_games_no_kick())
    {
        tPlayer.nGamesWithoutKicks = tAccountDefinition.num_public_games_no_kick();
    }

    if (tAccountDefinition.has_num_vote_kicks_initiated())
    {
        tPlayer.nKicksInitiated = tAccountDefinition.num_vote_kicks_initiated();
    }

    if (tAccountDefinition.has_times_vote_kicked())
    {
        tPlayer.nTimesVoteKicked = tAccountDefinition.times_vote_kicked();
    }

    tPlayer.tAccount.uPartitionFlags = tPartition.flags();


    if (tAccountDefinition.console_data().has_nfp_default_time())
    {
        tPlayer.tNFPDefaultScanTime = Blizzard::Time::FromUnixTime(tAccountDefinition.console_data().nfp_default_time());
    }
    else
    {
        tPlayer.tNFPDefaultScanTime = 0;
    }

    if (tAccountDefinition.console_data().has_nfp_goblin_time())
    {
        tPlayer.tNFPGoblinScanTime = Blizzard::Time::FromUnixTime(tAccountDefinition.console_data().nfp_goblin_time());
    }
    else
    {
        tPlayer.tNFPGoblinScanTime = 0;
    }
}
} // namespace SHero

namespace SHero {
static void sUpdateCurrencyDataPersistedFlag(Player &, const D3::Items::CurrencySavedData &); // DWARF SHero.cpp:2629.
static void sComposeAccountDefinition(
    Player & tPlayer,
    uint32 dwHeroSaveOptions,
    D3::Account::SavedDefinition & tDefinition)
{
    tDefinition.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));


    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        D3::Account::Digest & tDigest = *tDefinition.mutable_digest();
        tDigest.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));
        tPlayer.tAccount.tBanner.Export(*tDigest.mutable_banner_configuration());
        tDigest.mutable_last_played_hero_id()->set_id_high(0);
        tDigest.mutable_last_played_hero_id()->set_id_low(tPlayer.uHeroId);
        tDigest.set_flags(tPlayer.tAccount.uDigestFlags);
        tDigest.set_completed_solo_rift(!!tPlayer.bCompletedSoloRift);
        tDefinition.set_seen_tutorials(tPlayer.tSeenTutorials.GetRawBuffer(), tPlayer.tSeenTutorials.GetLength());
    }
    // Inference: default stash sequence follows Player.cpp:972 and the original unrolled comparisons.
    BOOL fDefaults = TRUE;
    for (int32 i = 0; i < 13; ++i)
    {
        if (tPlayer.bStashIcons[i] != i % MAX_STASH_TABS_CONSOLE)
        {
            fDefaults = FALSE;
            break;
        }
    }

    ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    SGame & tSGame = SGameManager::GameGet();
    BOOL bIsHardcore = (tPlayer.tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE)) != 0;
    BOOL bIsSeasonal = (tSGame.tStartParams.dwCreationFlags & (1u << GAMECREATEPARAM_SEASON)) != 0;

    D3AccountPartition ePartitionNum = GetPartition(bIsHardcore, bIsSeasonal);





    D3::Account::AccountPartition * ptPartition = tDefinition.mutable_partitions(0);



    tDefinition.mutable_saved_attributes()->clear_attributes();
    ptPartition->mutable_saved_attributes()->clear_attributes();


    AccountDefinition_SerializeAttributes(tACDPlayer, *tDefinition.mutable_saved_attributes(), ASM_ACCOUNT_ONLY);
    AccountDefinition_SerializeAttributes(tACDPlayer, *ptPartition->mutable_saved_attributes(), ASM_PARITION_ONLY);


    ptPartition->set_alt_level(tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL)));
    ptPartition->set_flags(tPlayer.tAccount.uPartitionFlags);


    ptPartition->clear_crafter_data();
    ptPartition->clear_currency_data();

    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        tPlayer.tCrafterSavedData.Export(*ptPartition->mutable_crafter_data(), &tDefinition);

        bool clearDirtyBitFlags = !(dwHeroSaveOptions & (1u << HSO_READ_ONLY));
        tPlayer.tCurrencySavedData.Export(
            *ptPartition->mutable_currency_data(),
            CurrencySavedData::EXPORT_PARTITION_INDIVIDUAL,
            CurrencySavedData::EXPORT_DIRTY_PERSIST,
            TRUE,
            clearDirtyBitFlags);

        tPlayer.tCurrencySavedData.Export(
            *tDefinition.mutable_account_wide_currency_data(),
            CurrencySavedData::EXPORT_PARTITION_ACCOUNT_WIDE,
            CurrencySavedData::EXPORT_DIRTY_PERSIST,
            TRUE,
            clearDirtyBitFlags);

        SPlayerConsumables &tConsumables = SPlayerGetConsumables(tPlayer);
        tConsumables.Export(
            *tDefinition.mutable_consumables(),
            (1u << DPF_DIRTY_PERSIST), clearDirtyBitFlags,
            // Inference: original immediate 0xc selects PERSIST_IN_PROGRESS | PERSISTED.
            (dwHeroSaveOptions & (1u << HSO_READ_ONLY)) ? 0 : ((1u << DPF_PERSIST_IN_PROGRESS) | (1u << DPF_PERSISTED)));


        if (!(dwHeroSaveOptions & (1u << HSO_READ_ONLY)))
        {
            sUpdateCurrencyDataPersistedFlag(tPlayer, ptPartition->currency_data());
            sUpdateCurrencyDataPersistedFlag(tPlayer, tDefinition.account_wide_currency_data());
        }

        tPlayer.tCosmeticItemSavedData.Export(&tDefinition);
        SPlayerExportDeliveredRewards(tPlayer, tDefinition.mutable_delivered_rewards());

        ptPartition->set_accepted_license_bits(tPlayer.uAcceptedLicenseBitsAccount);

        if (fDefaults)
        {
            ptPartition->clear_stash_icons();
        }
        else
        {
            ptPartition->set_stash_icons(tPlayer.bStashIcons, sizeof(tPlayer.bStashIcons));
        }
    }


    ptPartition->set_partition_id(ePartitionNum);

    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        tDefinition.set_num_vote_kicks_participated_in(tPlayer.nKicksParticipatedIn);
        tDefinition.set_num_public_games_no_kick(tPlayer.nGamesWithoutKicks);
        tDefinition.set_num_vote_kicks_initiated(tPlayer.nKicksInitiated);
        tDefinition.set_times_vote_kicked(tPlayer.nTimesVoteKicked);
    }

    // Inference: original slot mask 0x88000 selects shared stash and item sockets.
    uint32 uSlotsToSave = (1u << SLOT_PLAYER_SHARED_STASH) | (1u << SLOT_ITEM_SOCKETS);




    ptPartition->mutable_items()->clear_items();

    if (!(dwHeroSaveOptions & (1u << HSO_WEEKLY_CHALLENGE_DATA)))
    {
        ACDInventorySavedStateStore(tACDPlayer,

            *ptPartition->mutable_items()->mutable_items(), uSlotsToSave, true);
    }






    if (!GameIsBattleNetServer() && SGameGlobalsGet().ptAchievementsInterface) {
    // Inference: direct Console::ServerAchievementsInterface call on the globals base pointer requires this downcast.
        static_cast<Console::ServerAchievementsInterface*>(SGameGlobalsGet().ptAchievementsInterface)->PopulateAccountAchievementSnapshot(
            tPlayer.tAccount.tHandle, tDefinition);
    }





    tDefinition.mutable_console_data()->set_nfp_default_time(Blizzard::Time::ToUnixTime(tPlayer.tNFPDefaultScanTime));
    tDefinition.mutable_console_data()->set_nfp_goblin_time(Blizzard::Time::ToUnixTime(tPlayer.tNFPGoblinScanTime));

}
} // namespace SHero
namespace SHero {

static void sResetAccountDefinition(
    const Player & tPlayer,
    D3::Account::SavedDefinition & tDefinition)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    tDefinition = D3::Account::SavedDefinition::default_instance();
    tDefinition.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));
    AccountDefinition_DigestToDefaults(*tDefinition.mutable_digest());


    D3::Account::AccountPartition * ptPartition = tDefinition.mutable_partitions()->Add();

    *ptPartition->mutable_saved_attributes() = D3::AttributeSerializer::SavedAttributes::default_instance();
    ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(D3::GameBalance::Handle::default_instance().game_balance_type());
    ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_gbid(D3::GameBalance::Handle::default_instance().gbid());

    ptPartition->clear_items();
    ACDInventoryDeleteSharedStash(tACDPlayer);
}
}


















































static ACDID sResetPlayer(
    Player * ptPlayer,
    const GBID gbidHeroClass,
    const uint32 dwPlayerFlags,
    const uint32 dwSaveOptionBits,
    const BOOL bResetAllHeroData)
{
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);


    uint32 dwOldActorFlags = 0;
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACDPlayer.id));
    if (ptActor)
    {
        dwOldActorFlags = ptActor->dwActorFlags;
    }

    float flOldRunSpeedBonus = FastAttribGetValueFloat(tACDPlayer.idPassiveAttribGroup, ATTRIBUTE_MOVEMENT_SCALAR_UNCAPPED_BONUS);

    D3::Account::SavedDefinition tAccountDefinition;


    AccountDefinition_ToDefaults(tAccountDefinition);

    {
        D3AccountPartition idPartition = GetPartition((ptPlayer->tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE)) != 0, GameIsSeasonal());
        tAccountDefinition.mutable_partitions(0)->set_partition_id(idPartition);
    }










    ACDInventoryDeleteAll(tACDPlayer);


    ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        ACDInventoryDeleteAll(*ptACDHireling);
    }





    SPlayerRemoveFromSaveList(*ptPlayer);



    SPlayerKillAllPets(*ptPlayer, TRUE, TRUE, TRUE, FALSE);


    SPlayerClearEnemyList(*ptPlayer);


    HirelingStableDispose(ptPlayer->tHirelingStable);
    SHirelingSendStableInfoToClient(*ptPlayer);


    ptPlayer->dwBossEncounterFlags = 0;

    for (int32 i = 0; i < 4; ++i)
    {
        ptPlayer->flPVPPlayerDamageTaken[i] = 0.0f;
    }


    ptPlayer->uConsoleSyncFlags = ~0u;


    SNO snoHero = SNOFileLookupGUID(HERO_SNO, ptPlayer->szHeroFilename);
    if (snoHero != SNO_NONE)
    {
        gptGroupDefHero->SNODelete(snoHero);
    }




    WorldPlace tPlace = tACDPlayer.GetPlace();
    Quaternion q = tACDPlayer.GetOrientation();


    ActorID idActorPlayer = (ActorID)ACDGetOwner(ptPlayer->idPlayerACD);
    PlayerSetACD(*ptPlayer, ACDID_INVALID);
    ActorDelete(idActorPlayer);

    if (bResetAllHeroData)
    {
        HeroResetSavedData(*ptPlayer);


        for (int32 i = 0; i < 20; ++i)
        {
            ptPlayer->tCurrencySavedData.arCurrency[i].uFlags = 9;
        }
    }
    else
    {
        SigmaMemoryClear(&ptPlayer->tHeroStateData, sizeof(ptPlayer->tHeroStateData));
        ptPlayer->tHeroStateData.SetDefaults();
    }
    SPlayerSyncWaypointsToGame(*ptPlayer);

    D3::Hero::SavedDefinition tHeroDefinition;
    tHeroDefinition.mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(GB_HEROS);
    tHeroDefinition.mutable_saved_attributes()->mutable_gb_handle()->set_gbid(gbidHeroClass);

    D3::Items::Mails tMails;
    ACDID idACDPlayerNew = PlayerActorCreate(*ptPlayer, dwPlayerFlags, tPlace, q, TRUE, tHeroDefinition, tAccountDefinition, tMails, dwSaveOptionBits);










    SPlayer_ClientLoadComplete(*ptPlayer);

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        ptACDPlayer->MarkHeroStateCurrencyChanged();
        idActorPlayer = (ActorID)ptACDPlayer->idOwner;  // reuses idActorPlayer: DWARF has it in W23 over 0x730a10-0x730a60, loaded once (2228:25) for both ActorFlagSet calls


        if (dwOldActorFlags & (1u << ACTOR_GOD_MODE_BIT))
        {
            ActorFlagSet(idActorPlayer, ACTOR_GOD_MODE_BIT, TRUE);
        }

        if (dwOldActorFlags & (1u << ACTOR_EASYKILL_BIT))
        {
            ActorFlagSet(idActorPlayer, ACTOR_EASYKILL_BIT, TRUE);
        }

        FastAttribSetValueFloat(ptACDPlayer->idPassiveAttribGroup, ATTRIBUTE_MOVEMENT_SCALAR_UNCAPPED_BONUS, flOldRunSpeedBonus);
        ptACDPlayer->ModifiedMark(0);
    }




    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    SPlayerSendBanner(listDestPlayers, *ptPlayer);


    SPlayerAddToSaveList(*ptPlayer, 0, 0x2000000, PLAYER_SAVE_MODE_END_OF_FRAME);

    return idACDPlayerNew;
}
// LINKER-REMOVED: HeroToggleSex (DWARF SHero.cpp:2260, _Z13HeroToggleSex5ACDID, 0xa8 bytes, low_pc 0) is not in the binary: the linker removed it.
// It is written from its DWARF locals and surviving line table because its call keeps sResetPlayer out of line
// (two callers); columns: 2271:32 is hGB.gbid, 2273:50 uPlayerFlags. The sResetPlayer arguments 0/FALSE are inferred.
BOOL HeroToggleSex(
    ACDID idACDPlayer)
{
	Player *ptPlayer = PlayerGetByACD(idACDPlayer);
	if (!ptPlayer)
		return FALSE;




	ActorCommonData &tACD = ACDGet(ptPlayer->idPlayerACD);
	GBID gbidHeroClass = tACD.hGB.gbid;

	uint32 dwPlayerFlags = ptPlayer->tHeroStateData.uPlayerFlags;
	dwPlayerFlags ^= (1u << PLAYERFLAG_FEMALE);

	ACDID idACDPlayerNew =
	    sResetPlayer(ptPlayer, gbidHeroClass, dwPlayerFlags, 0, FALSE);

    return idACDPlayerNew != ACDID_INVALID;
}




ACDID HeroReset(
    ACDID idACDPlayer,
    const GBID gbidHeroClass,
    const uint32 dwSaveOptionBits,
    const BOOL bFemale,
    const BOOL bResetAllHeroData)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {




        uint32 dwPlayerFlags = ptPlayer->tHeroStateData.uPlayerFlags;
        dwPlayerFlags = bFemale
            ? (dwPlayerFlags | (1u << PLAYERFLAG_FEMALE))
            : (dwPlayerFlags & ~(1u << PLAYERFLAG_FEMALE));




        dwPlayerFlags &= ~((1u << PLAYERFLAG_TRIED_SALVAGE) | (1u << PLAYERFLAG_ENTERED_ACT1) | (1u << PLAYERFLAG_ENTERED_ACT2) | (1u << PLAYERFLAG_ENTERED_ACT3) | (1u << PLAYERFLAG_ENTERED_ACT4) | (1u << PLAYERFLAG_ENTERED_ACT5) | (1u << PLAYERFLAG_FREE_ENCHANT));

        ACDID idACDPlayerNew = sResetPlayer(ptPlayer, gbidHeroClass, dwPlayerFlags, dwSaveOptionBits, bResetAllHeroData);
        return idACDPlayerNew;
    }
    return ACDID_INVALID;
}



void HeroEnsureHeroHasAllSkillAttributes(
    ActorCommonData & tACD,
    const BOOL fNewHero)
{

    tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO)), 1);


    tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_PUNCH)), 1);


    tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_USE_ITEM)), 1);

    PowerActorAutoPurchaseAlwaysKnownTraits(tACD);

    if (fNewHero)
    {
        PowerActorAutoPurchaseStartingSkillsAndTraits(tACD);
    }
}


























































































































































































































namespace SHero {

void DigestUItoDigest(
    D3::Hero::Digest & digest,
    const D3::Hero::DigestUI & digestUI)
{

    digest.set_last_played_act(digestUI.last_played_act());
    digest.set_highest_unlocked_act(digestUI.highest_unlocked_act());
    digest.set_last_played_quest(digestUI.last_played_quest());
    digest.set_last_played_quest_step(digestUI.last_played_quest_step());
    digest.set_time_played(digestUI.time_played());
    digest.set_highest_solo_rift_completed(digestUI.highest_solo_rift_completed());


    if (digestUI.has_last_played_difficulty_deprecated())
    {
        digest.set_last_played_difficulty_deprecated(digestUI.last_played_difficulty_deprecated());
    }
    if (digestUI.has_highest_unlocked_difficulty_deprecated())
    {
        digest.set_highest_unlocked_difficulty_deprecated(digestUI.highest_unlocked_difficulty_deprecated());
    }
    if (digestUI.has_highest_completed_difficulty_deprecated())
    {
        digest.set_highest_completed_difficulty_deprecated(1);
    }
}



void DigesttoDigestUI(
    D3::Hero::DigestUI & digestUI,
    const D3::Hero::Digest & digest)
{

    digestUI.set_last_played_act(digest.last_played_act());
    digestUI.set_highest_unlocked_act(digest.highest_unlocked_act());
    digestUI.set_last_played_quest(digest.last_played_quest());
    digestUI.set_last_played_quest_step(digest.last_played_quest_step());
    digestUI.set_time_played(digest.time_played());
    digestUI.set_highest_solo_rift_completed(digest.highest_solo_rift_completed());


    if (digest.has_last_played_difficulty_deprecated())
    {
        digestUI.set_last_played_difficulty_deprecated(digest.last_played_difficulty_deprecated());
    }
    if (digest.has_highest_unlocked_difficulty_deprecated())
    {
        digestUI.set_highest_unlocked_difficulty_deprecated(digest.highest_unlocked_difficulty_deprecated());
    }
    if (digest.has_highest_completed_difficulty_deprecated())
    {
        digestUI.set_highest_completed_difficulty_deprecated(1);
    }
}



void SetHandicapValues(
    ActorCommonData & tACD)
{
    float flMagicFindFromHandicap;
    float flGoldFindFromHandicap;
    float flXPBonusFromHandicap;
    GameGetPlayerHandicapValues(GameGetHandicapLevel(), flMagicFindFromHandicap, flGoldFindFromHandicap, flXPBonusFromHandicap);
    tACD.AttributesSetFloat(ATTRIBUTE_MAGIC_FIND_HANDICAP, flMagicFindFromHandicap);
    tACD.AttributesSetFloat(ATTRIBUTE_GOLD_FIND_HANDICAP, flGoldFindFromHandicap);
    tACD.AttributesSetFloat(ATTRIBUTE_EXPERIENCE_BONUS_PCT_HANDICAP, flXPBonusFromHandicap);
}
} // namespace SHero
namespace SHero {

static void sUpdateCurrencyDataPersistedFlag(
    Player & tPlayer,
    const D3::Items::CurrencySavedData & tCurrencySavedData)
{

    for (int32 i = 0; i < tCurrencySavedData.currency_size(); ++i)
    {
        const D3::Items::CurrencyData & tCurrencyData = tCurrencySavedData.currency(i);
        BOOL bSetValue = tCurrencyData.count() != 0; // Inference: conditional bit-3 update from original and/orr/csel.
        if (bSetValue) tPlayer.tCurrencySavedData.arCurrency[tCurrencyData.id()].uFlags |= (1u << DPF_PERSISTED); else tPlayer.tCurrencySavedData.arCurrency[tCurrencyData.id()].uFlags &= ~(1u << DPF_PERSISTED);
    }

}
} // namespace SHero
