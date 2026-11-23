#include "ConsolePlayerResolver.h"
#include "../../Client/Console/Online.h"
#include "../../Common/Console/ConsoleLicenses.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Partition.h"
#include "../../Common/Definitions/HeroDefinition.h"
// DWARF Online.h: PartyFindPlayer and PartyGetPlayerAccountAndHero declarations.
namespace Console {
namespace Online {
int32 PartyFindPlayer(GameNetwork::ConnectionRef ptGameConnection, int32 nControllerIndex, BOOL & bIsPrimary, BOOL & bIsGuest, BOOL & bIsLocal, char * szUsername, uint32 uUsernameLen);
BOOL PartyGetPlayerAccountAndHero(int32 nPlayerIndex, D3::Account::SavedDefinition * ptAccountDefinition, D3::Hero::SavedDefinition * ptHeroDefinition);
}
}

namespace Console {





















PlayerResolver::PlayerResolver()
    : OnlineService::PlayerResolver(0, "Console::PlayerResolver")
{
}




PlayerResolver::~PlayerResolver()
{
}



BOOL PlayerResolver::LoadFromParty(GameNetwork::ConnectionRef ptConnection)

{
    OnlineService::PlayerResolveData & tResolveData = GetResolvedPlayer(0);



    tResolveData.nPartyIndex = Online::PartyFindPlayer(
        ptConnection,
        static_cast<uint8>(tResolveData.nControllerIndex),
        tResolveData.bIsPrimary,
        tResolveData.bIsGuest,
        tResolveData.bIsLocal,
        tResolveData.szAccountName, sizeof(tResolveData.szAccountName));

    if (tResolveData.nPartyIndex == -1)
    {
        return FALSE;
    }

    if (GameIsWeeklyChallenge())
    {
        Online::GameParams tParams;
        if (!Online::PartyGetGameParams(tParams))
        {
            return FALSE;
        }


        if (!tParams.m_tWeeklyChallengeData.has_account_snapshot() ||
            !tParams.m_tWeeklyChallengeData.has_hero_snapshot() ||
            !tParams.m_tWeeklyChallengeData.has_rift_snapshot())
        {
            return FALSE;
        }


        tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_ACCOUNT_DEFINITION;
        tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_HERO_DEFINITION;

        tResolveData.tAccountDefinition = tParams.m_tWeeklyChallengeData.account_snapshot();
        tResolveData.tHeroDefinition = tParams.m_tWeeklyChallengeData.hero_snapshot();
        tResolveData.tHeroDefinition.mutable_digest()->mutable_hero_id()->set_id_low(tResolveData.uHeroId);

        D3::Account::SavedDefinition tTempAccountDefinition;
        BOOL bSuccess = Online::PartyGetPlayerAccountAndHero(
            tResolveData.nPartyIndex, &tTempAccountDefinition, NULL);



        // The original continues regardless of bSuccess; no conditional branch follows the call.
        tResolveData.uLastChallengeRewardEarned = tTempAccountDefinition.digest().challenge_rift_account_data().last_challenge_reward_earned();
        tResolveData.tAccountDefinition.set_seen_tutorials(tTempAccountDefinition.seen_tutorials());
        tResolveData.tAccountDefinition.mutable_digest()->set_completed_solo_rift(tTempAccountDefinition.digest().completed_solo_rift());
        tResolveData.tHeroDefinition.mutable_saved_data()->mutable_saved_conversations()->CopyFrom(tTempAccountDefinition.digest().challenge_rift_account_data().saved_conversations());

        if (tTempAccountDefinition.console_data().challenge_rift_personal_best_rift_number() == tParams.m_tWeeklyChallengeConfig.challenge_number())
        {
            for (const D3::Account::ConsoleChallengeRiftPersonalBest & tPersonalBest : tTempAccountDefinition.console_data().challenge_rift_personal_best())
            {
                tResolveData.tChallengeRiftPersonalBests.push_back(blz::pair<uint64, uint64>(tPersonalBest.id(), tPersonalBest.score()));
            }
        }
        return TRUE;


    }

    BOOL bSuccess = Online::PartyGetPlayerAccountAndHero(
        tResolveData.nPartyIndex,
        &tResolveData.tAccountDefinition,
        &tResolveData.tHeroDefinition);
    if (!bSuccess || !tResolveData.tHeroDefinition.has_digest())
    {
        return FALSE;
    }


    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_ACCOUNT_DEFINITION;
    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_HERO_DEFINITION;

    bool bGameIsSeasonal = GameIsSeasonal() != FALSE;
    bool bGameIsHardcore = (tResolveData.tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE)) != 0;
    bool bPartitionIsSeasonal = IsPartitionSeasonal((D3AccountPartition)tResolveData.tAccountDefinition.partitions(0).partition_id());
    bool bPartitionIsHardcore = IsPartitionHardcore((D3AccountPartition)tResolveData.tAccountDefinition.partitions(0).partition_id());




    if (bGameIsSeasonal != bPartitionIsSeasonal)
    {
        return FALSE;
    }


    if (bGameIsHardcore != bPartitionIsHardcore)
    {
        return FALSE;
    }


    if (tResolveData.uConsoleLicenses & (1 << LICENSE_NECROMANCER))
    {
        OnlineServiceServer::License tLicense;
        tLicense.id = 17614; // DWARF LICENSE_BNET_ACCOUNT_D3_IN_GAME_NECROMANCER, absent from src declarations.
        tLicense.expireTime = 0;

        tResolveData.tRawLicenses.push_back(tLicense);
    }

    return TRUE;



}




void PlayerResolver::ResolvePlayerData(uint32 uIndex)

{
    OnlineService::PlayerResolveData & tResolveData = GetResolvedPlayer(uIndex);
    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_MAIL;
}


}
