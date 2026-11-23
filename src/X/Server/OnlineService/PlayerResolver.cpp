// Original file: X/Server/OnlineService/PlayerResolver.cpp
#include "PlayerResolver.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/OnlineService/OnlineServiceLogStream.h"
#include "../../Sigma/ErrorManager/ErrorManagerLogging.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Sigma/UnicodeText.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
// DWARF X/Server/OnlineService/OnlineServiceServer.cpp:1169 (no declaration in this CU).
namespace OnlineServiceServer {
OnlineService::ErrorCode ComposeContentLicenses(const OnlineServiceServer::LicenseArray & tLicenses, OnlineService::ContentLicenseArray & tContentLicenses);
}
// DWARF BaseResolver.h:77 inline body; the src header only declares it.
inline const blz::string & OnlineService::BaseResolver::GetLogHeaderStdString() const { return m_logHeader; }










// PlayerResolveData(): initialiser-list lines 46-68 come from the line table; members without a row
// (tMails, the license arrays, tChallengeRiftPersonalBests, uLastChallengeRewardEarned) are left implicit.






static XVarScope s_scope("PlayerResolver", NULL);

XVarBool s_varValidatePlayers(s_scope, "ValidatePlayers", true);




XVarBool s_varLoadMail(s_scope, "LoadMail", true);
namespace OnlineService {
PlayerResolveData::PlayerResolveData()
    : uHeroId(0)
    , uBnetAccountId(0)
    , tAccountId()
    , eTeam(TEAM_INVALID)
    , tAccountDefinition()
    , tHeroDefinition()
    , uResolveResultFlags(0)
    , uAuthToken(0)


    , uSessionFlags(0)

    , nControllerIndex(-1)
    , nPartyIndex(-1)

    , bIsPrimary(FALSE)
    , bIsGuest(FALSE)
    , bIsLocal(FALSE)
    , uConsoleLicenses(0)
    , eLocale(XLOCALE_INVALID)
    , idServerSocket(SERVERSOCKETID_INVALID)
    , uJoinFlags(0)
    , tAdded(0)
    , nTeamId(0)
    , teamRating(0.0f)
    , teamVariance(0.0f)
    , individualRating(0.0f)
    , individualVariance(0.0f)
{
}



PlayerResolver::PlayerResolver(
    uint32 uResolveOptions,
    const char * pszResolverName)
    : BaseResolver(pszResolverName)
    , m_tResolveArray()
    , m_tResolveCompleteCb()
    , m_gameId(0)
    , m_weeklyChallengeId(0)
    , m_challengeOverrideUsed(false)
    , m_uResolveOptions(uResolveOptions)
{
}



PlayerResolver::~PlayerResolver()
{
}



void PlayerResolver::AddPlayer(
    HeroId uHeroId,
    const GameAccountHandle & tAccountId,
    const char * pszAccountName,
    Team eTeam,
    uint64 uAuthToken)
{
    PlayerResolveData tResolveData;
    tResolveData.uHeroId = uHeroId;
    tResolveData.tAccountId = tAccountId;
    Blizzard::String::Copy(tResolveData.szAccountName, pszAccountName, sizeof(tResolveData.szAccountName));
    tResolveData.eTeam = eTeam;
    tResolveData.uAuthToken = uAuthToken;

    tResolveData.eLocale = XLOCALE_INVALID;
    tResolveData.idServerSocket = SERVERSOCKETID_INVALID;
    tResolveData.uJoinFlags = 0;
    tResolveData.tAdded = Blizzard::Time::Seconds();

    if (uAuthToken == 0)
    {

        tResolveData.uAuthToken = GenerateAuthToken();
    }
    m_tResolveArray.Add_Tracked(tResolveData);
    // Line 127 col 29 (teamRating) zeroes nTeamId and the four ratings after the Add; order from AddPvpPlayer lines 166-170.
    tResolveData.nTeamId = 0;
    tResolveData.teamRating = 0.0f;
    tResolveData.teamVariance = 0.0f;
    tResolveData.individualRating = 0.0f;
    tResolveData.individualVariance = 0.0f;
}













































void PlayerResolver::Begin(
    PlayerResolver::ResolveCompleteCb cb,
    uint64 gameId)
{
    m_gameId = gameId;
    m_tResolveCompleteCb = cb;


    for (int32 i=0; i<m_tResolveArray.GetArraySize(); ++i)
    {
        ResolvePlayerData(i);

        PlayerResolveData & tResolveData = m_tResolveArray[i];
        if (!(m_uResolveOptions & (1 << RESOLVE_OPTION_RESOLVE_LICENSES)))
        {

            ErrorCode cerrCode = OnlineServiceServer::ComposeContentLicenses(OnlineServiceServer::LicenseArray(), tResolveData.tContentLicenses);


            if (cerrCode != ERROR_GLOBAL_SUCCESS)
            {
                m_errorCode = cerrCode;
            }
        }
    }

    CheckAllPlayersResolved();
}



void PlayerResolver::CheckAllPlayersResolved()

{

    for (int32 i=0; i<m_tResolveArray.GetArraySize(); ++i)
    {
        PlayerResolveData & tResolveData = m_tResolveArray[i];



        if (!(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_ACCOUNT_DEFINITION)) || !(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_HERO_DEFINITION)))
        {
            return;
        }













        if ((m_uResolveOptions & (1 << RESOLVE_OPTION_ACCOUNT_LOCK)) && !(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_ACCOUNT_LOCK)))
        {
            return;
        }


        if ((m_uResolveOptions & (1 << RESOLVE_OPTION_RESOLVE_BNET_ACCOUNT)) && !(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_BNET_ACCOUNT)))
        {
            return;
        }



        if ((m_uResolveOptions & (1 << RESOLVE_OPTION_RESOLVE_LICENSES)) && !(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_LICENSES)))
        {
            return;
        }



        if ((m_uResolveOptions & (1 << RESOLVE_OPTION_CHALLENGE_OVERRIDE)) && !(tResolveData.uResolveResultFlags & (1 << RESOLVE_RESULT_CHALLENGE_OVERRIDDEN)))
        {
            return;
        }
    }





    ResolveCompleteInternal();
}

// Only the SGame lookup of lines 270-348 survives in the binary; the rest of the
// original body (lines 274-347) left no instructions and is not reconstructed.
ErrorCode PlayerResolver::ValidatePlayers(
    SGameID idSGame)
{
    SGame * ptSGame = SGameManager::GameTryToGet(idSGame);
    if (ptSGame == NULL)
    {
        return ERROR_GAMESESSIONJOIN_INVALID_GAME;







































































    }
    return ERROR_GLOBAL_SUCCESS;
}





































































ErrorCode PlayerResolver::AppendPlayersToGame(
    SGameID idSGame)
{
    SGame * ptSGame = SGameManager::GameTryToGet(idSGame);
    if (ptSGame == NULL)
    {
        return ERROR_GAMESESSIONJOIN_GENERIC_FAILURE;
    }



    PlayerResolveData tPendingResolveData;
    for (int32 i=0; i<m_tResolveArray.GetArraySize(); ++i)
    {

        if (ptSGame->tPendingJoinMap.Lookup(m_tResolveArray[i].uHeroId, tPendingResolveData))
        {

            ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, GetLogHeaderStdString() << ": " << GameIDLabel(idSGame) << " PlayerResolver::AppendPlayersToGame: Player already in game!  accountId=" << m_tResolveArray[i].tAccountId << " heroId=" << m_tResolveArray[i].uHeroId << " authToken=" << m_tResolveArray[i].uAuthToken);
            return ERROR_GAMESESSIONJOIN_EXISTINGRESERVATION;
        }






        BOOL bDummyBool;
        if (ptSGame->tKickMap.Lookup(m_tResolveArray[i].tAccountId, bDummyBool))
        {

            ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, GetLogHeaderStdString() << ": " << GameIDLabel(idSGame) << " PlayerResolver::AppendPlayersToGame: Player accountId exists in game Kickmap!  accountId=" << m_tResolveArray[i].tAccountId << " heroId=" << m_tResolveArray[i].uHeroId << " authToken=" << m_tResolveArray[i].uAuthToken);
            return ERROR_GAMESESSIONJOIN_KICKLIST_DENIAL;
        }
    }






    for (int32 i=0; i<m_tResolveArray.GetArraySize(); ++i)
    {
        ptSGame->tPendingJoinMap.SetAt_Tracked(m_tResolveArray[i].uHeroId, m_tResolveArray[i]);








    }

    return ERROR_GLOBAL_SUCCESS;
}
void PlayerResolver::ResolveCompleteInternal()
{




    if (m_tResolveCompleteCb)
    {
        m_tResolveCompleteCb(m_errorCode);

    }
}

}  // namespace OnlineService
