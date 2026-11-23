// X/Client/CHero.cpp
// Functions sit on their original line numbers.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CHero.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "CPlayer.h"
#include "../Common/Player.h"
#include "Gfx/GfxVB.h"
#include "../Common/Act.h"
#include "../Common/GameSwig.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/Partition.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/Definitions/HeroDefinition.h"
#include "../Common/Definitions/QuestDefinition.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/Protobuf/Profile.pb.h"
#include "../Common/Protobuf/Console.pb.h"
#include "../Common/Protobuf/ChallengeRifts.pb.h"
#include "../Common/Protobuf/GameMessage.pb.h"
#include "../Server/SGameStatsSwig.h"
#include "../Common/Achievements/Console/ConsoleAchievements.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Shell/CmdLine.h"
#include "Console/GamerProfile.h"
#include "Console/Online.h"
#include "Cutscene.h"
#include "../Common/OnlineService/OnlineService.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Common/Definitions/AccountDefinition.h"
#include "../Common/VisualInventory.h"
#include "../Common/GameConstants.h"
#include "../Common/HirelingSwig.h"















namespace CHero {
enum {
    MINIMUM_TIMER_SAVE_INTERVAL_MS = 240000,
};

struct UserSaveRecord {  // size 0x4
    UserSaveRecord()
    {
        uLastTimeSave = 0;
    }
    Blizzard::Time::Millisecond uLastTimeSave;  // 0x00
};








UserSaveRecord s_arSaveRecords[4];















XGrowableMap<int, int, int, int> stEligibleQuestSNOs;

static const Act arActList[] = { ACT_1, ACT_2, ACT_3, ACT_4, ACT_5 };
}  // namespace CHero











void CHero::InitializeForGame()
{
    for (uint32 i = 0; i < 4; ++i)
    {
        s_arSaveRecords[i].uLastTimeSave = 0;
    }

    stEligibleQuestSNOs.RemoveAll();
    for (int32 i = 0; i < sizeof(arActList) / sizeof(arActList[0]); ++i)
    {
        SNO snoAct = ActSNOFromActEnum(arActList[i]);
        ScopedActDefinitionLockR ptDefAct(snoAct);
        if (!ptDefAct)
        {
            return;
        }

        int32 nNumQuest = ActGetNumQuests(ptDefAct);
        for (int32 quest = 0; quest < nNumQuest; ++quest)
        {
            SNO snoQuest = ptDefAct->arQuests[quest].snoQuest;
            if (snoQuest != SNO_NONE && QuestGetQuestType(snoQuest) == 0)
            {
                stEligibleQuestSNOs.SetAt_Tracked(snoQuest, QuestGetNumSteps(snoQuest));
            }
        }
    }
}





void CHero::DisposeForGame()
{
}





static D3::Account::AccountPartition & sGetAccountPartitionForHero(D3::Account::SavedDefinition & tDefinition, BOOL bIsHardcore, BOOL bIsSeasonal)
{
    D3AccountPartition ePartition = GetPartition(bIsHardcore, bIsSeasonal);
    return *tDefinition.mutable_partitions(ePartition);
}










static D3::Profile::ClassInfo* sGetPlayerClassInfo(
    D3::Profile::AccountProfile& tAccountProfile,
    const GBID gbid)
{
    PlayerClass eClass = PlayerClassFromGBID(gbid);

    switch (eClass)
    {
    case PLAYER_CLASS_BARBARIAN:
        return tAccountProfile.mutable_class_barbarian();

    case PLAYER_CLASS_DEMONHUNTER:
        return tAccountProfile.mutable_class_demonhunter();

    case PLAYER_CLASS_MONK:
        return tAccountProfile.mutable_class_monk();

    case PLAYER_CLASS_WITCHDOCTOR:
        return tAccountProfile.mutable_class_witchdoctor();

    case PLAYER_CLASS_WIZARD:
        return tAccountProfile.mutable_class_wizard();

    case PLAYER_CLASS_CRUSADER:
        return tAccountProfile.mutable_class_crusader();

    case PLAYER_CLASS_NECROMANCER:
        return tAccountProfile.mutable_class_necromancer();

    default:
        // The fatal's row and its line argument (0xc2) are both line 194.
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        return NULL;
    }
}


// Declarations this file needs from headers not yet in src/ (named per declaration).

















int32 AccountDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey, int32 nDefault);  // X/Common/Definitions/AccountDefinition.cpp

CutsceneID CutsceneGetFullScreenCutscene();  // X/Client/Cutscene.cpp

namespace Console { namespace Online {
void PartyLeaveAndRemake(uint32 uFlags);  // X/Client/Console/Online.cpp
BOOL LobbyIsConnected(int32 nUserIndex);  // X/Client/Console/LobbyService.cpp
} }

namespace Console { namespace HeroPublishing {
void Publish(int32 nControllerIndex, D3::Account::SavedDefinition & tAccountDefinition, D3::Hero::SavedDefinition & tHeroDefinition, BOOL bForcePublish);  // X/Client/Console/HeroPublishing.cpp
} }

namespace Console { namespace Online {
void LeaderboardPostAchievementScore(int32 nUserIndex, uint32 uScore, Console::Online::LeaderboardPostCallback tCallback);  // X/Client/Console/Online.cpp
void LeaderboardPostConquest(int32 nUserIndex, OnlineService::AchievementId idAchievement, Console::Online::LeaderboardPostCallback tCallback);  // X/Client/Console/Online.cpp
} }

BOOL ParseProtocolBufferMessage(GameMessageType eType, uint8 * pMessageBuffer, google::protobuf::MessageLite & tMessage);  // X/Common/Networking/GameMessage.cpp

void AccountDefinition_SerializeAttributes(ActorCommonData & tACDPlayer, D3::AttributeSerializer::SavedAttributes & tSavedAttributes, AccountSerializeMode eMode);  // X/Common/Definitions/AccountDefinition.cpp:320
void HeroDefinitionSaveAttributes(ActorCommonData & tACDPlayer, D3::AttributeSerializer::SavedAttributes & tSavedAttributes);  // X/Common/Definitions/HeroDefinition.cpp:511
void ACDInventorySavedStateStore(const ActorCommonData & tACD, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, uint32 uSlotsToSaveBits, bool bDirtyOnly);  // X/Common/ActorInventory.cpp:5822
void ACDHirelingInventorySavedStateStore(Player & tPlayer, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, bool bDirtyOnly, bool bSaveSocketedItems);  // X/Common/ActorInventory.cpp:5908
namespace UIWaypointMap { Act GetWaypointActForPlayer(const Player * ptPlayer); }  // X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp:124
namespace Console {
namespace Online {
void PartyUpdateCachedLocalPlayerAccountAndHero(int32 nPartyIndex, const D3::Account::SavedDefinition &, const D3::Hero::SavedDefinition &);  // X/Client/Console/Online.cpp:4469
}
}
































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































void CHero::Save(int32 nControllerIndex, uint32 uReason)


{
    if (nControllerIndex < 0 || nControllerIndex >= 4)
    {
        return;
    }


    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (!PlayerIsLocal(ptPlayer) ||
        !ptPlayer->bDidFirstTimeSetup ||
        ptPlayer->bHasPerformedFinalSaveForQuit)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(nControllerIndex))
    {
        return;
    }


    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(nControllerIndex);


    uint32 uPlayerFlags = tHeroDefinition.digest().player_flags();
    if ((uPlayerFlags & 1) && (uPlayerFlags & 8))
    {
        return;
    }



    if (!Validate(nControllerIndex) &&
        !Console::Online::PartyIsHost())
    {

        Console::Online::PartyLeaveAndRemake(1);
        GameStartMenu(400);
        return;
    }


    Synchronize(nControllerIndex);



    const uint32 uUpdateProgressReasons = PlayerSaveReason_Console | PlayerSaveReason_ServerQuit | PlayerSaveReason_PlayerQuit | PlayerSaveReason_GameEnd | PlayerSaveReason_Quest;





    if ((uReason & uUpdateProgressReasons) &&
        !GameIsWeeklyChallenge())
    {
        sComputeProgress(tAccountDefinition, tHeroDefinition);
    }



    const uint32 uImmediatePublishReasons = PlayerSaveReason_ServerQuit | PlayerSaveReason_PlayerQuit | PlayerSaveReason_GameEnd;



    const uint32 uSkipPublishingReasons = PlayerSaveReason_Console;



    if (!(uReason & uSkipPublishingReasons) &&
        !GameIsWeeklyChallenge())
    {
        Console::HeroPublishing::Publish(nControllerIndex, tAccountDefinition, tHeroDefinition, uReason & uImmediatePublishReasons);
    }



    uint32 uImmediatePlayerSaveReasons = PlayerSaveReason_Console | PlayerSaveReason_AcctFlags | PlayerSaveReason_Diablo | PlayerSaveReason_ServerQuit | PlayerSaveReason_PlayerQuit | PlayerSaveReason_Checkpoint | PlayerSaveReason_GameEnd | PlayerSaveReason_LevelUp | PlayerSaveReason_Quest;

















    UserSaveRecord & tSaveRecord = s_arSaveRecords[nControllerIndex];
    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();



    if (Blizzard::Time::GetTimeElapsed(tSaveRecord.uLastTimeSave, now) >= MINIMUM_TIMER_SAVE_INTERVAL_MS &&
        CutsceneGetFullScreenCutscene() == CUTSCENEID_INVALID)
    {
        uImmediatePlayerSaveReasons |= PlayerSaveReason_Timer;
    }


    if (uPlayerFlags & 1)
    {
        uImmediatePlayerSaveReasons |= PlayerSaveReason_Dead;
    }


    if (uReason & uImmediatePlayerSaveReasons)
    {

        Console::GamerProfile::Save(nControllerIndex, Console::GamerProfile::SaveCallback());
        tSaveRecord.uLastTimeSave = now;

        if (uReason & PlayerSaveReason_PlayerQuit)
        {
            ptPlayer->bHasPerformedFinalSaveForQuit = TRUE;
        }
    }

}




void CHero::OnSavedQuestUpdate(Player * ptPlayer, const PlayerSavedQuestUpdateMessage & tUpdateMsg)


{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest & tDigest = *tHeroDefinition.mutable_digest();

    tDigest.set_last_played_act(tUpdateMsg.eLastPlayedAct);
    tDigest.set_last_played_quest(tUpdateMsg.snoLastPlayedQuest);
    tDigest.set_last_played_quest_step(tUpdateMsg.nLastPlayedStepUID);

    if (tHeroDefinition.has_saved_data())
    {

        D3::Hero::SavedData & tSavedData = *tHeroDefinition.mutable_saved_data();

        tSavedData.set_main_quest_handicap_snapshot(tUpdateMsg.nHandicapLevel);
    }

    tHeroDefinition.clear_saved_quest();
    for (uint32 i = 0; i < tUpdateMsg.dwNumQuests; ++i)
    {
        tUpdateMsg.tSavedQuest[i].Export(*tHeroDefinition.add_saved_quest());
    }
}




void CHero::OnQuestHistoryUpdate(Player * ptPlayer, const PlayerQuestHistoryUpdateMessage & tUpdateMsg)


{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest & tDigest = *tHeroDefinition.mutable_digest();

    tDigest.clear_quest_history();
    for (uint32 i = 0; i < tUpdateMsg.dwNumEntries; ++i)
    {
        tUpdateMsg.tQuestHistory[i].Export(*tDigest.add_quest_history());
    }
}




void CHero::OnQuestRewardHistoryUpdate(Player * ptPlayer, const PlayerQuestRewardHistoryUpdateMessage & tUpdateMsg)


{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    tHeroDefinition.clear_quest_reward_history();
    ptPlayer->tHeroStateData.nNumQuestRewardEntries = tUpdateMsg.nNumQuestRewardEntries;
    for (int32 i = 0; i < tUpdateMsg.nNumQuestRewardEntries; ++i)
    {
        tUpdateMsg.tQuestRewardHistory[i].Export(*tHeroDefinition.add_quest_reward_history());
        ptPlayer->tHeroStateData.tQuestRewardHistory[i] = tUpdateMsg.tQuestRewardHistory[i];
    }
}




void CHero::OnSavedConversationUpdate(Player * ptPlayer, const PlayerSavedConversationsUpdateMessage & tUpdateMsg)


{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
        tUpdateMsg.tSavedConversations.Export(*tAccountDefinition.mutable_digest()->mutable_challenge_rift_account_data()->mutable_saved_conversations());
    }
    else
    {
        D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
        D3::Hero::SavedData & tSavedData = *tHeroDefinition.mutable_saved_data();

        tUpdateMsg.tSavedConversations.Export(*tSavedData.mutable_saved_conversations());
    }
}




void CHero::OnAchievementsAwarded(Player * ptPlayer, const D3::GameMessage::AchievementsAwardedConsole & tUpdateMsg)
{

    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4) return;
    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex)) return;





    bool bSeasonalScoreAwarded = false;






    Console::ClientAchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    Console::Achievements * ptAchievements = sgtAppGlobals.ptAchievements;



    for (OnlineService::AchievementId idAchievement : tUpdateMsg.achievements())
    {


        ptAchievementsInterface->OnAchievementAwarded(ptPlayer->nControllerIndex, tUpdateMsg);

        const OnlineService::AchievementDefinition * ptAwardedAchvDef = ptAchievements->GetAchievementDefinition(idAchievement);


        if (ptAwardedAchvDef &&
            GameIsSeasonal() &&
            OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE &&
            GameGetCurrentSeason() == Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex).digest().season_id() &&
            ptAchievements->IsAchievementValidForSeason(ptAwardedAchvDef, GameGetCurrentSeason()))
        {

            if (ptAwardedAchvDef->m_flags & (1 << 18))
            {
                Console::Online::LeaderboardPostConquest(ptPlayer->nControllerIndex, idAchievement, Console::Online::LeaderboardPostCallback());
            }


            if (ptAchievements->ShouldCountAchievementPoints(ptAwardedAchvDef))
                bSeasonalScoreAwarded |= ptAwardedAchvDef->m_pointsValue != 0;
        }
    }




    if (bSeasonalScoreAwarded)
    {
        uint32 uAchievementPointsTotal = 0;
        blz::vector<OnlineService::AchievementRecord> arAchievementRecords;
        sgtAppGlobals.ptAchievementsInterface->FindAwardedAchievements(ptPlayer->nControllerIndex, arAchievementRecords);


        for (const OnlineService::AchievementRecord & tRecord : arAchievementRecords)
        {
            const OnlineService::AchievementDefinition * ptAchvDef = ptAchievements->GetAchievementDefinition(tRecord.m_achievementId);
            if (ptAchievements->ShouldCountAchievementPoints(ptAchvDef) && ptAchievements->IsAchievementValidForSeason(ptAchvDef, GameGetCurrentSeason()))
                uAchievementPointsTotal += ptAchvDef->m_pointsValue;
        }



        if (uAchievementPointsTotal != 0)
        {



            Console::Online::LeaderboardPostAchievementScore(ptPlayer->nControllerIndex, uAchievementPointsTotal, Console::Online::LeaderboardPostCallback());
        }
    }
}




void CHero::OnAchievementCriteriaUpdate(int32 nControllerIndex, const D3::GameMessage::AchievementsCriteriaUpdateConsole & tUpdateMsg)


{
    if (nControllerIndex < 0 || nControllerIndex >= 4 ||

        !Console::Online::PartyIsInParty(nControllerIndex))
    {
        return;
    }

    if (!Console::GamerProfile::IsHeroLoaded(nControllerIndex))
    {
        return;
    }



    sgtAppGlobals.ptAchievementsInterface->OnAchievementCriteriaUpdate(nControllerIndex, tUpdateMsg);
}



void CHero::OnGameFinished(Player * ptPlayer)

{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest * tDigest = tHeroDefinition.mutable_digest();

    ptPlayer->tHeroStateData.eLastPlayedAct = ACT_1;
    ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints = 0;
    ptPlayer->tHeroStateData.tSavedData.tLastSavePoint.Reset();
    tDigest->set_last_played_quest(SNO_NONE);

    SNO snoAct1 = ActSNOFromActEnum(ACT_1);
    ScopedActDefinitionLockR ptAct1(snoAct1);

    if (ptAct1 && ptAct1->serQuests.ElementCount(sizeof(ActQuestInfo)) > 0)
    {
        tDigest->set_last_played_quest(ptAct1->arQuests[0].snoQuest);
    }
    tDigest->set_last_played_quest_step(-1);
    tDigest->set_last_played_act(ptPlayer->tHeroStateData.eLastPlayedAct);
}



void CHero::OnAdventureModeUnlocked(Player * ptPlayer)

{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Account::Digest * tDigest = tAccountDefinition.mutable_digest();
    uint32 flags = tDigest->flags();

    BOOL bHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;
    flags |= bHardcore ? (1 << D3::Account::Digest_Flags_HARDCORE_ADVENTURE_MODE_UNLOCKED) : (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_UNLOCKED);
    tDigest->set_flags(flags);
}



void CHero::OnAccountFlagsSync(
    Player * ptPlayer,
    uint32 uAccountFlags)
{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Account::Digest * tDigest = tAccountDefinition.mutable_digest();
    tDigest->set_flags(uAccountFlags);

}



void CHero::OnCounterIncrement(Player * ptPlayer, const ConsoleCounterIncrementMessage & tIncrementMsg)


{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Profile::HeroProfile & tHeroProfile = *tHeroDefinition.mutable_console_data()->mutable_hero_profile();

    D3::Console::Profile & tConsoleProfile = Console::GamerProfile::GetProfile(ptPlayer->nControllerIndex);
    D3::Profile::AccountProfile & tAccountProfile = *tConsoleProfile.mutable_account_profile();
    uint32 uValue = tIncrementMsg.uValue;

    switch (tIncrementMsg.nCounter)
    {
    case STAT_COUNTER_PLAYER_MONSTERS_KILLED:

        tHeroProfile.set_monsters_killed((tHeroProfile.has_monsters_killed() ? tHeroProfile.monsters_killed() : 0) + uValue);
        tAccountProfile.set_monsters_killed((tAccountProfile.has_monsters_killed() ? tAccountProfile.monsters_killed() : 0) + uValue);

        if (ptPlayer->tHeroStateData.uPlayerFlags & 1)
        {

            tAccountProfile.set_hardcore_monsters_killed((tAccountProfile.has_hardcore_monsters_killed() ? tAccountProfile.hardcore_monsters_killed() : 0) + uValue);
        }
        break;

    case STAT_COUNTER_PLAYER_ELITES_KILLED:

        tHeroProfile.set_elites_killed((tHeroProfile.has_elites_killed() ? tHeroProfile.elites_killed() : 0) + uValue);
        tAccountProfile.set_elites_killed((tAccountProfile.has_elites_killed() ? tAccountProfile.elites_killed() : 0) + uValue);
        break;

    case STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP:

        tHeroProfile.set_gold_collected((tHeroProfile.has_gold_collected() ? tHeroProfile.gold_collected() : 0) + uValue);
        tAccountProfile.set_gold_collected((tAccountProfile.has_gold_collected() ? tAccountProfile.gold_collected() : 0) + uValue);
        break;

    case STAT_COUNTER_PLAYER_QUEST_COMPLETED:

        tAccountProfile.set_loot_runs_completed((tAccountProfile.has_loot_runs_completed() ? tAccountProfile.loot_runs_completed() : 0) + uValue);
        break;
    }






}



void CHero::SyncRandomSeeds(
    Player * ptPlayer,
    GenericBlobMessage * ptMessage)
{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    D3::Hero::ConsoleRandomVendorSeedList tRandomSeedData;

    if (!ParseProtocolBufferMessage(ptMessage->tHeader.eType, (uint8 *)ptMessage,
        tRandomSeedData))
    {
        return;
    }





    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    tHeroDefinition.mutable_console_data()->mutable_random_item_vendor_seed_list()->CopyFrom(tRandomSeedData);
}



void CHero::SyncRandomCubeSeeds(
    Player * ptPlayer,
    GenericBlobMessage * ptMessage)
{
    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4)
    {
        return;
    }


    D3::Account::ConsoleRandomTransmuteSeedList tRandomSeedData;

    if (!ParseProtocolBufferMessage(ptMessage->tHeader.eType, (uint8 *)ptMessage,
        tRandomSeedData))
    {
        return;
    }





    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    if (GameIsWeeklyChallenge())
    {
        return;
    }

    BOOL bIsHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Account::AccountPartition & tPartition = sGetAccountPartitionForHero(tAccountDefinition, bIsHardcore, GameIsSeasonal());

    tPartition.mutable_console_partition_data()->mutable_console_random_transmute_seed_list()->CopyFrom(tRandomSeedData);
}




void CHero::OnCharacterKilled(Player * ptPlayer, SNO snoKiller, MonsterRarity eKillerRarity, const GBID * pRareNameGBIDs, const CRefString & usKiller)
{





    if (!PlayerIsLocal(ptPlayer)) return;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4) return;





    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex)) return;





    if (GameIsWeeklyChallenge()) return;




    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::ConsoleData & tHeroConsoleData = *tHeroDefinition.mutable_console_data();
    D3::Profile::HeroProfile & tHeroProfile = *tHeroConsoleData.mutable_hero_profile();

    if ((ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) && !(ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_DECEASED)))
    {

        tHeroProfile.set_death_time(Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp()));
        D3::Profile::KillerInfo & tKillerInfo = *tHeroProfile.mutable_killer_info();

        tKillerInfo.set_sno_killer(snoKiller);
        tKillerInfo.set_rarity(eKillerRarity);
        if (eKillerRarity == 2)
        {
            for (int i = 0; i < 2; i++)
            {
                tKillerInfo.add_rare_name_gbids(pRareNameGBIDs[i]);
            }
        }

        tHeroConsoleData.set_killer_name(usKiller);
    }
}



void CHero::OnChallengeRiftComplete(int32 nUserIndex, uint64 idLeaderboard, uint32 uRiftNumber, uint64 uScore)




{
    if (!Console::GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    D3::Account::SavedDefinition & tAccount = Console::GamerProfile::GetAccount(nUserIndex);

    if (tAccount.console_data().challenge_rift_personal_best_rift_number() > uRiftNumber)
    {
        return;
    }


    if (tAccount.console_data().challenge_rift_personal_best_rift_number() != uRiftNumber)
    {

        tAccount.mutable_console_data()->set_challenge_rift_personal_best_rift_number(uRiftNumber);
        tAccount.mutable_console_data()->clear_challenge_rift_personal_best();
    }


    for (D3::Account::ConsoleChallengeRiftPersonalBest & tBest : *tAccount.mutable_console_data()->mutable_challenge_rift_personal_best())
    {
        if (tBest.id() == idLeaderboard)
        {
            if (uScore > tBest.score())
            {
                tBest.set_score(uScore);
                tBest.set_timestamp(Console::Online::GetNetworkTimestamp());
                Console::GamerProfile::Save(nUserIndex, Console::GamerProfile::SaveCallback());
            }
            return;
        }
    }



    D3::Account::ConsoleChallengeRiftPersonalBest & tBest = *tAccount.mutable_console_data()->add_challenge_rift_personal_best();
    tBest.set_id(idLeaderboard);
    tBest.set_score(uScore);
    tBest.set_timestamp(Console::Online::GetNetworkTimestamp());
    Console::GamerProfile::Save(nUserIndex, Console::GamerProfile::SaveCallback());
}




void CHero::OnChallengeRiftRewardEarned(int32 nUserIndex, D3::Account::ConsoleChallengeRiftReward tReward)


{
    if (!Console::GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    if (!GameIsWeeklyChallenge())
    {
        return;
    }

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(nUserIndex);

    if (tReward.challenge_rift() > tAccountDefinition.digest().challenge_rift_account_data().last_challenge_reward_earned())
    {



        tAccountDefinition.mutable_digest()->mutable_challenge_rift_account_data()->set_last_challenge_reward_earned(tReward.challenge_rift());

        tReward.set_create_time(Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp()));
        if (Console::Online::LobbyIsConnected(nUserIndex) &&
            OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE)
        {
            tReward.set_season_earned(OnlineService::GetSeasonNum());
        }

        *tAccountDefinition.mutable_console_data()->add_challenge_rift_reward() = tReward;

        Console::GamerProfile::Save(nUserIndex, Console::GamerProfile::SaveCallback());
    }
}



void CHero::OnChallengeRiftRewardClaimed(int32 nUserIndex, uint32 uRewardId)


{
    if (!Console::GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(nUserIndex);

    for (int32 i = 0; i < tAccountDefinition.console_data().challenge_rift_reward_size(); ++i)
    {

        if (tAccountDefinition.console_data().challenge_rift_reward(i).challenge_rift() == uRewardId)
        {
            tAccountDefinition.mutable_console_data()->mutable_challenge_rift_reward()->DeleteSubrange(i, 1);
            break;
        }
    }
}





BOOL CHero::Validate(int32 nControllerIndex)

{







    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return TRUE;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (!PlayerIsLocal(ptPlayer)) return TRUE;
    if (ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4 || ptPlayer->nControllerIndex != nControllerIndex)
    {
        return TRUE;
    }




    if (!Console::GamerProfile::IsHeroLoaded(nControllerIndex))
    {
        return TRUE;
    }


    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return TRUE;
    }


    if (GameIsWeeklyChallenge())
    {
        return TRUE;
    }

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    const D3::Hero::Digest & tHeroDigest = tHeroDefinition.digest();





































    { int32 nExisting = tHeroDigest.gbid_class(); int32 nNew = ptACDPlayer->hGB.gbid; if (nNew != nExisting) { return FALSE; } }


    BOOL bIsFemale = (tHeroDigest.player_flags() & 2) != 0;
    BOOL bNewIsFemale = (ptPlayer->tHeroStateData.uPlayerFlags & 2) != 0;
    if (bIsFemale != bNewIsFemale) { return FALSE; }


    BOOL bIsHardcore = tHeroDigest.player_flags() & 1;
    BOOL bNewIsHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;
    if (bNewIsHardcore != bIsHardcore) { return FALSE; }


    BOOL bIsSeasonal = tAccountDefinition.digest().has_season_id() && tHeroDigest.season_created() == tAccountDefinition.digest().season_id();
    BOOL bNewIsSeasonal = GameIsSeasonal(); if (bNewIsSeasonal != bIsSeasonal) { return FALSE; }



    int32 nLevel = tHeroDigest.level();
    int32 nNewLevel = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL)); if (nNewLevel < nLevel) { return FALSE; }












    const D3::Account::AccountPartition & tPartition = sGetAccountPartitionForHero(tAccountDefinition, bIsHardcore, GameIsSeasonal());


    int32 nAltLevel = tPartition.alt_level();
    int32 nNewAltLevel = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL)); if (nNewAltLevel < nAltLevel) { return FALSE; }













    { int32 nExisting = AccountDefinitionSavedAttributeGetInt(tPartition.saved_attributes(), FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS), 0); int32 nNew = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS)); if (nNew < nExisting) { return FALSE; } }








    return TRUE;
}
void CHero::Synchronize(
    int32 nControllerIndex)
{
    Player* ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (!PlayerIsLocal(ptPlayer) ||
        ptPlayer->nControllerIndex != nControllerIndex ||
        ptPlayer->nControllerIndex < 0 || ptPlayer->nControllerIndex >= 4 ||

        !ptPlayer->bDidFirstTimeSetup)
    {
        return;
    }

    if (GameIsWeeklyChallenge())
    {
        D3::Account::SavedDefinition& tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
        tAccountDefinition.set_seen_tutorials(ptPlayer->tSeenTutorials.GetRawBuffer(), ptPlayer->tSeenTutorials.GetLength());
        return;
    }

    if (!Console::GamerProfile::IsHeroLoaded(ptPlayer->nControllerIndex))
    {
        return;
    }


    BOOL bIsHardcore = ptPlayer->tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_HARDCORE);
    BOOL bIsSeasonal = GameIsSeasonal();

    D3::Account::SavedDefinition& tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Account::Digest& tAccountDigest = *tAccountDefinition.mutable_digest();
    D3::Account::AccountPartition& tPartition = sGetAccountPartitionForHero(tAccountDefinition, bIsHardcore, bIsSeasonal);

    D3::Hero::SavedDefinition& tHeroDefinition = Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest& tHeroDigest = *tHeroDefinition.mutable_digest();
    D3::Hero::SavedData& tSavedData = *tHeroDefinition.mutable_saved_data();
    D3::Hero::ConsoleData& tConsoleData = *tHeroDefinition.mutable_console_data();
    D3::Account::ConsoleData& tConsoleAccountData = *tAccountDefinition.mutable_console_data();

    D3::Console::Profile& tProfile = Console::GamerProfile::GetProfile(ptPlayer->nControllerIndex);
    D3::Profile::AccountProfile& tAccountProfile = *tProfile.mutable_account_profile();

    HeroStateData& tHeroStateData = ptPlayer->tHeroStateData;

    if (tHeroDigest.hero_id().id_low() != ptPlayer->uHeroId)
    {
        return;
    }








    tAccountDefinition.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));
    tAccountDigest.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));

    tHeroDefinition.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));
    tHeroDigest.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));


    if (ptPlayer->uLastPlayedTimeUpdate != 0)
    {
        uint32 now = GameGetServerTime();
        uint32 uTicksSinceLastUpdate = now - ptPlayer->uLastPlayedTimeUpdate;
        tHeroStateData.tSavedData.uSecondsPlayed += (uTicksSinceLastUpdate / TICKS_PER_SECOND);
        ptPlayer->uLastPlayedTimeUpdate = now - (uTicksSinceLastUpdate % TICKS_PER_SECOND);
    }

    tAccountDefinition.set_seen_tutorials(ptPlayer->tSeenTutorials.GetRawBuffer(), ptPlayer->tSeenTutorials.GetLength());

    tAccountDefinition.set_num_vote_kicks_participated_in(0);
    tAccountDefinition.set_num_public_games_no_kick(0);
    tAccountDefinition.set_num_vote_kicks_initiated(0);
    tAccountDefinition.set_times_vote_kicked(0);
    tAccountDefinition.clear_account_wide_transmog_data();

    tPartition.clear_crafter_data();
    D3::ItemCrafting::CrafterSavedData& tCrafterSavedData = *tPartition.mutable_crafter_data();
    ptPlayer->tCrafterSavedData.Export(tCrafterSavedData, &tAccountDefinition);

    tPartition.clear_currency_data();
    ptPlayer->tCurrencySavedData.Export(
        *tPartition.mutable_currency_data(),
        CurrencySavedData::EXPORT_PARTITION_INDIVIDUAL, CurrencySavedData::EXPORT_DIRTY_PERSIST, FALSE, 0);



    tAccountDefinition.clear_account_wide_currency_data();
    ptPlayer->tCurrencySavedData.Export(
        *tAccountDefinition.mutable_account_wide_currency_data(),
        CurrencySavedData::EXPORT_PARTITION_ACCOUNT_WIDE, CurrencySavedData::EXPORT_DIRTY_PERSIST, FALSE, 0);



    tAccountDefinition.clear_account_wide_cosmetic_item_data();
    ptPlayer->tCosmeticItemSavedData.Export(&tAccountDefinition);

    tPartition.set_flags(ptPlayer->tAccount.uPartitionFlags);

    tHeroDefinition.set_accepted_license_bits(ptPlayer->uAcceptedLicenseBitsHero);

    tSavedData.clear_boss_kill_flags();
    for (int32 i = 0; i < 6; ++i)
    {
        tSavedData.add_boss_kill_flags(tHeroStateData.uBossKillFlags[i]);
    }

    tHeroDigest.set_player_flags(tHeroStateData.uPlayerFlags);
    tHeroDigest.set_highest_unlocked_act(tHeroStateData.eHighestUnlockedAct);
    tConsoleData.set_last_played_handicap(GameGetHandicapLevel());
    if (GameIsAdventureMode())
    {
        tConsoleData.set_last_played_adventure_mode_act(UIWaypointMap::GetWaypointActForPlayer(ptPlayer));
    }
    else
    {
        tConsoleData.clear_last_played_adventure_mode_act();
    }

    uint32 uDeltaTimePlayed = 0;
    if (tHeroStateData.tSavedData.uSecondsPlayed > tHeroDigest.time_played())
    {

        uDeltaTimePlayed = tHeroStateData.tSavedData.uSecondsPlayed - tHeroDigest.time_played();
    }

    tHeroDigest.set_time_played(tHeroStateData.tSavedData.uSecondsPlayed);
    tHeroDigest.set_last_played_time(Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp()));


    tSavedData.clear_potion_button_item();

    if (tHeroStateData.tSavedData.tPotionBtn.m_annItem != ANN_INVALID)
    {
        ActorCommonData* ptACDItem = ACDTryToGet(ACDFromANN(tHeroStateData.tSavedData.tPotionBtn.m_annItem));
        if (ptACDItem)
        {
            const InventoryLocation* ptInvLoc = ptACDItem->TryToGetInventoryLocation();
            if (ptInvLoc && ptInvLoc->eSlot != SLOT_PLAYER_BASE_HEALTHPOTION)
            {
                D3::Hero::SavedItemLink* ptItemLoc = tSavedData.mutable_potion_button_item();
                ptItemLoc->set_x(ptInvLoc->pt.x);
                ptItemLoc->set_y(ptInvLoc->pt.y);
            }
        }
    }









    ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
    if (idACDHireling != ACDID_INVALID)
    {
        ActorCommonData& tACDHireling = ACDGet(idACDHireling);
        HirelingClass eClass = HirelingGetClass(tACDHireling);


        HirelingInfo& tInfo = ptPlayer->tHirelingStable.tHireling[eClass].tInfo;


        tInfo.eClass        = eClass;
        tInfo.gbidName      = tACDHireling.pRareNameGBIDs[0];
        tInfo.fDead         = !tACDHireling.IsAlive();

        // 5: the size of HirelingInfo::arnPowerKeyParam (five -1 stores at 2437).
        for (uint32 i = 0; i < 5; i++)
        {
            tInfo.arnPowerKeyParam[i] = -1;
        }

        AttribGroupID idFastAttrib = tACDHireling.AttributesGetAttributeGroup();
        FastAttribIterator tIt(idFastAttrib, TRUE);
        while (FastAttribIterate(tIt))
        {
            FastAttribKey tKey = tIt.tEntryBuffer.tKey;
            FastAttribValue tValue = tIt.tEntryBuffer.tValue;
            if (KeyGetAttrib(tKey) != ATTRIBUTE_SKILL || tValue.nValue == 0)
            {
                continue;
            }


            int32 nPowerParam = KeyGetParam(tKey);
            int32 nRow, nColumn;
            if (HirelingPowerGetRowColumn(tACDHireling, nPowerParam, nRow, nColumn))
            {

                tInfo.arnPowerKeyParam[nRow] = nPowerParam;
            }
        }
    }


    for (int32 i = 0; i < NUM_HIRELING_CLASSES; ++i)
    {
        tHeroStateData.tSavedData.tHirelingSavedData.tHireling[i] = ptPlayer->tHirelingStable.tHireling[i].tInfo;
    }


    D3::Hero::EquipmentManagerData& tEquipmentManagerSavedData = *tHeroDefinition.mutable_equipment_manager_data();

    tEquipmentManagerSavedData.clear_loadouts();

    for (int32 nLoadout = 0; nLoadout < 5; ++nLoadout)
    {
        D3::Hero::Loadout& tSavedLoadout = *tEquipmentManagerSavedData.add_loadouts();
        const LoadoutSavedData& tPlayerLoadout = ptPlayer->tLoadouts[nLoadout];

        tPlayerLoadout.Export(tSavedLoadout, ptPlayer->idPlayerACD);
    }









    ActorCommonData* ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return;
    }


    tHeroStateData.tSavedData.Export(*ptPlayer, ptACDPlayer->hGB.gbid, tSavedData, TRUE);








    tAccountDefinition.mutable_saved_attributes()->clear_attributes();
    AccountDefinition_SerializeAttributes(*ptACDPlayer, *tAccountDefinition.mutable_saved_attributes(), ASM_ACCOUNT_ONLY);

    tPartition.mutable_saved_attributes()->clear_attributes();
    AccountDefinition_SerializeAttributes(*ptACDPlayer, *tPartition.mutable_saved_attributes(), ASM_PARITION_ONLY);


    D3::AttributeSerializer::SavedAttributes& tHeroAttributes = *tHeroDefinition.mutable_saved_attributes();
    tHeroAttributes.clear_attributes();
    HeroDefinitionSaveAttributes(*ptACDPlayer, tHeroAttributes);




    int32 nHeroLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    int32 nAltLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
    tHeroDigest.set_level(nHeroLevel);
    tHeroDigest.set_deprecated_alt_level(nAltLevel);

    tPartition.set_alt_level(nAltLevel);
    tHeroDigest.set_pvp_rank(ptACDPlayer->AttributesGetInt(ATTRIBUTE_PVP_RANK));
    tHeroDigest.set_highest_solo_rift_completed(ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL));


    if (!tAccountDigest.completed_solo_rift() &&
        (ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL) > 0 ||
         tHeroDigest.highest_solo_rift_completed() > 0))
    {
        tAccountDigest.set_completed_solo_rift(true);
    }












    uint32 uSlotsToSave = (1 << SLOT_PLAYER_SHARED_STASH);


    tPartition.mutable_items()->clear_items();
    ACDInventorySavedStateStore(*ptACDPlayer,

        *tPartition.mutable_items()->mutable_items(), uSlotsToSave, true);



    Avenger::Console::SaveAvengerInfoToAccount(ptPlayer,

        *tConsoleAccountData.mutable_avenger_data());


    tConsoleAccountData.set_nfp_default_time(Blizzard::Time::ToUnixTime(ptPlayer->tNFPDefaultScanTime));
    tConsoleAccountData.set_nfp_goblin_time(Blizzard::Time::ToUnixTime(ptPlayer->tNFPGoblinScanTime));






    ptACDPlayer->ptVisualInventory->tEquipmentCurrent.Export(*tHeroDigest.mutable_visual_equipment());

    tHeroDefinition.mutable_items()->clear_items();




    ACDInventorySavedStateStore(*ptACDPlayer,

        *tHeroDefinition.mutable_items()->mutable_items(), ~uSlotsToSave, true);




    ACDHirelingInventorySavedStateStore(*ptPlayer,

        *tHeroDefinition.mutable_items()->mutable_items(), true, true);








    DifficultyLevel eDifficulty = GameGetDifficultyLevel();

    if (!tAccountProfile.has_deprecated_highest_difficulty() ||
        eDifficulty > (int32)tAccountProfile.deprecated_highest_difficulty())
    {
        tAccountProfile.set_deprecated_highest_difficulty(eDifficulty);
    }









    if ((tHeroStateData.uBossKillFlags[eDifficulty] & (1 << BOSSKILLFLAG_BUTCHER)) && (!tAccountProfile.has_highest_boss_difficulty_1() || eDifficulty > (int32)tAccountProfile.highest_boss_difficulty_1())) tAccountProfile.set_highest_boss_difficulty_1(eDifficulty);
    if ((tHeroStateData.uBossKillFlags[eDifficulty] & (1 << BOSSKILLFLAG_BELIAL)) && (!tAccountProfile.has_highest_boss_difficulty_2() || eDifficulty > (int32)tAccountProfile.highest_boss_difficulty_2())) tAccountProfile.set_highest_boss_difficulty_2(eDifficulty);
    if ((tHeroStateData.uBossKillFlags[eDifficulty] & (1 << BOSSKILLFLAG_AZMODAN)) && (!tAccountProfile.has_highest_boss_difficulty_3() || eDifficulty > (int32)tAccountProfile.highest_boss_difficulty_3())) tAccountProfile.set_highest_boss_difficulty_3(eDifficulty);
    if ((tHeroStateData.uBossKillFlags[eDifficulty] & (1 << BOSSKILLFLAG_DIABLO)) && (!tAccountProfile.has_highest_boss_difficulty_4() || eDifficulty > (int32)tAccountProfile.highest_boss_difficulty_4())) tAccountProfile.set_highest_boss_difficulty_4(eDifficulty);
    if ((tHeroStateData.uBossKillFlags[eDifficulty] & (1 << BOSSKILLFLAG_MALTHAEL)) && (!tAccountProfile.has_highest_boss_difficulty_5() || eDifficulty > (int32)tAccountProfile.highest_boss_difficulty_5())) tAccountProfile.set_highest_boss_difficulty_5(eDifficulty);

    if (bIsHardcore)
    {
        if (!tAccountProfile.has_highest_hardcore_level() ||
            nHeroLevel > tAccountProfile.highest_hardcore_level())
        {
            tAccountProfile.set_highest_hardcore_level(nHeroLevel);
        }
    }
    else
    {
        if (!tAccountProfile.has_highest_level() ||
            nHeroLevel > tAccountProfile.highest_level())
        {
            tAccountProfile.set_highest_level(nHeroLevel);
        }
    }

    D3::Profile::ClassInfo* ptClassInfo = sGetPlayerClassInfo(tAccountProfile, tHeroDigest.gbid_class());
    if (ptClassInfo)  // inferred: uNewTimePlayed's lexical block (2637-2649) needs braces; no instruction (the helper never returns NULL)
    {
        uint64 uNewTimePlayed = (ptClassInfo->has_playtime() ? ptClassInfo->playtime() : 0) + uDeltaTimePlayed;
        ptClassInfo->set_playtime(uNewTimePlayed);

        if (!ptClassInfo->has_highest_level() ||
            nHeroLevel > (int32)ptClassInfo->highest_level())
        {
            ptClassInfo->set_highest_level(nHeroLevel);
        }

        if (!ptClassInfo->has_deprecated_highest_difficulty() ||
            eDifficulty > (int32)ptClassInfo->deprecated_highest_difficulty())
        {
            ptClassInfo->set_deprecated_highest_difficulty(eDifficulty);
        }
    }


    if (GameIsSeasonal())
    {
        uint32 nCurrentSeason = GameGetCurrentSeason();




        OnlineService::Achievements& tAchievements = *sgtAppGlobals.ptAchievements;
        Console::ClientAchievementsInterface* ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

        OnlineService::AchievementId idSeasonJourneyRank = tAchievements.GetHighestEarnedSeasonJourneyAchievement(nCurrentSeason,

            Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex), ptAchievementsInterface);


        if (idSeasonJourneyRank)
        {
            tAccountProfile.set_season_journey_rank(idSeasonJourneyRank);
        }
        else
        {
            tAccountProfile.clear_season_journey_rank();
        }
    }



    sgtAppGlobals.ptAchievementsInterface->PopulateAccountAchievementSnapshot(nControllerIndex, tAccountDefinition);









    Console::Online::PartyUpdateCachedLocalPlayerAccountAndHero(
        ptPlayer->nPlayerIndex, tAccountDefinition, tHeroDefinition);


}





void CHero::sComputeProgress(D3::Account::SavedDefinition & tAccountDefinition, D3::Hero::SavedDefinition & tHeroDefinition)


{
    int32 nQuestsCompleted = 0;


    for (int32 nQuest = 0; nQuest < tHeroDefinition.digest().quest_history_size(); ++nQuest)
    {
        const D3::Hero::QuestHistoryEntry & tQuestHistory = tHeroDefinition.digest().quest_history(nQuest);
        SNO snoQuest = tQuestHistory.sno_quest();
        int32 nStepUID = tQuestHistory.highest_played_quest_step();
        int32 nNumQuestSteps;

        if (stEligibleQuestSNOs.Lookup(snoQuest, nNumQuestSteps) &&
            QuestStepIsCompletionStep(snoQuest, nStepUID))
        {
            ++nQuestsCompleted;
        }
    }

    float fCompletion = Math::Min(0.99f, (float)nQuestsCompleted / (float)stEligibleQuestSNOs.Count());


    if (nQuestsCompleted == stEligibleQuestSNOs.Count())
    {
        fCompletion = 1.0f;
    }

    if (!tAccountDefinition.console_data().has_progress() ||
        fCompletion > tAccountDefinition.console_data().progress())
    {
        tAccountDefinition.mutable_console_data()->set_progress(fCompletion);
    }
}
