// X/Client/Console/Matchmaking.cpp
// The DemonWare matchmaking sessions on the console builds: registering the local game,
// searching for public games, and the lobby tasks behind them.
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityID.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdDML/bdDMLInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdMatchMakingInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionID.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionParams.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdDML/bdDML.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdMatchMaking.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTask.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyErrorCodes.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdNet.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../../Common/GameCommonData.h"
#include "GamerProfile.h"
#include "LobbyService.h"
#include "Matchmaking.h"
#include <string.h>
#include <strings.h>

namespace Console { namespace Utilities { XLocale GetSystemLanguage(); } }  // X/Client/Console/UtilitiesNX64.cpp

















namespace Console {
namespace Online {
enum {
    GAME_PRIVATE = 0,
    GAME_PUBLIC = 1,
    REPUTATION_GOOD = 0,
    REPUTATION_BAD = 1,
};

enum QuickMatchQueryType {
    QUERY_QM_QUEST = 1,
    QUERY_QM_ACT = 2,
    QUERY_QM_ANY = 3,
};

enum QuickMatchQueryMode {
    QUERY_QM_NORMAL = 0,
    QUERY_QM_HARDCORE = 1,
    QUERY_QM_SEASONAL = 2,
    QUERY_QM_SEASONAL_HARDCORE = 3,
    QUERY_QM_CHALLENGE_RIFT = 4,
};

enum {
    MATCHMAKING_REGISTER_RETRY_DELAY_MS = 60000,
    MATCHMAKING_UPDATE_GAME_DELAY_MS = 2000,
};
class MatchMakingInfo : public bdMatchMakingInfo {  // size 0x180
public:

    MatchMakingInfo()
        : m_memberLevel(0),
          m_memberAct(-1),
          m_memberQuest(-1),
          m_memberMode(0),
          m_memberOpenSlots(0),
          m_memberLanguage(0),
          m_memberRegion(0),
          m_memberMonsterPower(0),
          m_memberTag(0),
          m_memberPublic(0),
          m_memberReputation(0)
    {
    }

    MatchMakingInfo(bdCommonAddrRef ptAddr,
                    bdSecurityID tSecID,
                    bdSecurityKey tSecKey)
        : bdMatchMakingInfo(ptAddr, 0, 4),
          m_memberLevel(0),
          m_memberAct(-1),
          m_memberQuest(-1),
          m_memberSecurityID(tSecID),
          m_memberSecurityKey(tSecKey),
          m_memberMode(0),
          m_memberOpenSlots(0),
          m_memberLanguage(0),
          m_memberRegion(0),
          m_memberMonsterPower(0),
          m_memberTag(0),
          m_memberPublic(0),
          m_memberReputation(0)
    {
    }




    virtual bdBool serialize(bdByteBuffer & buffer) const;
    virtual bdBool deserialize(bdByteBufferRef buffer);


    virtual bdUInt sizeOf()
    {
        return sizeof(MatchMakingInfo);
    }

    bdInt32 m_memberLevel;
    bdInt32 m_memberAct;
    bdInt32 m_memberQuest;
    bdSecurityID m_memberSecurityID;
    bdSecurityKey m_memberSecurityKey;
    bdInt32 m_memberMode;
    bdUInt32 m_memberOpenSlots;
    bdInt32 m_memberLanguage;
    bdInt32 m_memberRegion;
    bdInt32 m_memberMonsterPower;
    bdInt32 m_memberTag;
    bdInt32 m_memberPublic;
    bdInt32 m_memberReputation;
};




class QuickMatchQuery : public bdSessionParams {  // size 0x48
public:

    QuickMatchQuery()
        : m_memberparam_Level(0),
          m_memberparam_LevelMin(0),
          m_memberparam_LevelMax(0),
          m_memberparam_Act(-1),
          m_memberparam_Quest(-1),
          m_memberparam_Mode(0),
          m_memberparam_NumSlots(0),
          m_memberparam_Language(0),
          m_memberparam_Region(0),
          m_memberparam_MonsterPower(0),
          m_memberparam_Tag(0),
          m_memberparam_Reputation(0)
    {
    }

    virtual bdBool serialize(bdByteBuffer & buffer) const;


    virtual bdUInt sizeOf()
    {
        return sizeof(QuickMatchQuery);
    }

    bdInt32 m_memberparam_Level;
    bdInt32 m_memberparam_LevelMin;
    bdInt32 m_memberparam_LevelMax;
    bdInt32 m_memberparam_Act;
    bdInt32 m_memberparam_Quest;
    bdInt32 m_memberparam_Mode;
    bdUInt32 m_memberparam_NumSlots;
    bdInt32 m_memberparam_Language;
    bdInt32 m_memberparam_Region;
    bdInt32 m_memberparam_MonsterPower;
    bdInt32 m_memberparam_Tag;
    bdInt32 m_memberparam_Reputation;
};

class MatchmakerInternal : public Matchmaker {  // size 0x58
public:
    MatchmakerInternal();
    ~MatchmakerInternal();

    void IdleInternal();








    virtual BOOL IsRegistered()
    {
        return m_ptGameInfo != NULL;
    }

    virtual void Search(MatchParameters & tParams, SearchCallback pfnCallback);
    virtual BOOL IsSearching();
    virtual uint32 GetSearchResults(MatchmakerResults * ptResults, uint32 uMaxResults);
    virtual void Cancel();
    virtual void RegisterGame(const bdSecurityID & tID, const bdSecurityKey & tKey, const MatchParameters & tParams);
    virtual void UnregisterGame();
    virtual void UpdateGame(const bdSecurityID & tID, const bdSecurityKey & tKey, const MatchParameters & tParams);
    virtual MatchmakerRegion GetCurrentRegion(MatchmakerRegion eRegionPreference);

    void GetCountry();
    void OnGotCountry(const bdDMLInfo & tInfo);
    int32 GetCurrentLanguage();






























    void OnFailSessionUpdate()
    {
        ::delete m_ptGameInfo;
        m_ptGameInfo = NULL;
        m_uSessionUpdateFailedAt = Blizzard::Time::Milliseconds();
    }

    BOOL m_bConnected;
    LobbyTaskRef m_ptSearchTask;
    MatchMakingInfo * m_ptGameInfo;
    bdSessionID m_idSession;
    Blizzard::Time::Millisecond m_uSessionUpdateFailedAt;
    Blizzard::Time::Millisecond m_uGameInfoUpdatedAt;
    BOOL m_bGameInfoPlayersChanged;
    char m_szCountryCode[3];
};










class CreateSessionTask : public LobbyTask {  // size 0x1c8
public:


    CreateSessionTask(LobbyService * ptLobbyService, const MatchMakingInfo & tInfo)
        : LobbyTask(ptLobbyService),
          m_tInfo(tInfo)
    {
    }

    virtual void Start();
    virtual void OnComplete();
    virtual void OnFailed();

    MatchMakingInfo m_tInfo;
    bdSessionID m_idSession;
};










class UpdateSessionTask : public LobbyTask {  // size 0x1a8
public:


    UpdateSessionTask(LobbyService * ptLobbyService, const MatchMakingInfo & tInfo, BOOL bUpdatePlayers)
        : LobbyTask(ptLobbyService),
          m_tInfo(tInfo),
          m_bUpdatePlayers(bUpdatePlayers)
    {
    }

    virtual void Start();
    virtual void OnFailed();

    MatchMakingInfo m_tInfo;
    BOOL m_bUpdatePlayers;
};









class DeleteSessionTask : public LobbyTask {  // size 0x20
public:


    DeleteSessionTask(LobbyService * ptLobbyService)
        : LobbyTask(ptLobbyService)
    {
    }

    virtual void Start();
    virtual void OnComplete();
    virtual void OnFailed();
};









class SearchTask : public LobbyTask {  // size 0x1e70
public:


    SearchTask(LobbyService * ptLobbyService, const MatchParameters & tParams, SearchCallback pfnCallback)
        : LobbyTask(ptLobbyService),
          m_tMatchParams(tParams),
          m_ptQuery(NULL),
          m_uNumResults(0),
          m_pfnCallback(pfnCallback)
    {
    }

    virtual void Start();
    virtual void OnComplete();
    virtual void OnFailed();
    virtual void OnCancel();

    MatchParameters m_tMatchParams;
    bdSessionParams * m_ptQuery;
    MatchMakingInfo m_tResults[20];
    uint32 m_uNumResults;
    SearchCallback m_pfnCallback;
};



















class GetDMLUserInfoTask : public LobbyTask {  // size 0x160
public:


    GetDMLUserInfoTask(LobbyService * ptLobbyService, DMLCallback pfnCallback)
        : LobbyTask(ptLobbyService),
          m_pfnCallback(pfnCallback)
    {
    }

    virtual void Start();
    virtual void OnComplete();
    virtual void OnFailed();

private:
    bdDMLInfo m_tUserData;
    DMLCallback m_pfnCallback;
};














inline QuickMatchQueryMode MatchParamsToQMMode(const MatchParameters & tParams)
{
    if (tParams.eGameType == GT_CHALLENGE_RIFTS)
        return QUERY_QM_CHALLENGE_RIFT;

    if (tParams.bSeasonal)
        return tParams.bHardcore ? QUERY_QM_SEASONAL_HARDCORE : QUERY_QM_SEASONAL;

    return tParams.bHardcore ? QUERY_QM_HARDCORE : QUERY_QM_NORMAL;
}

















MatchmakerInternal * sptMatchmaker = NULL;






MatchParameters::MatchParameters()
    : bIsPublic(FALSE),
      eGameType(1),
      nLevel(0),
      bHardcore(FALSE),
      bSeasonal(FALSE),
      eAct(-1),
      snoQuest(-1),
      nHandicap(0),
      uNumPlayers(0),
      eRegion(REGION_DEFAULT),
      nLanguage(0),
      nTag(0),
      bBadReputation(FALSE)
{
}




void Matchmaker::Create()
{
    sptMatchmaker = new MatchmakerInternal();
}






Matchmaker * Matchmaker::Get()
{
    return sptMatchmaker;
}






void Matchmaker::Idle()
{
    sptMatchmaker->IdleInternal();
}





MatchmakerInternal::MatchmakerInternal()
    : m_bConnected(FALSE),
      m_ptGameInfo(NULL),
      m_uSessionUpdateFailedAt(0),
      m_uGameInfoUpdatedAt(0),
      m_bGameInfoPlayersChanged(FALSE)
{
    SigmaMemoryClear(m_szCountryCode, sizeof(m_szCountryCode));
}














void MatchmakerInternal::IdleInternal()
{
    BOOL bWasConnected = m_bConnected;

    int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();

    m_bConnected = (nPrimaryUserIndex != -1) ? LobbyService::Get(nPrimaryUserIndex)->IsConnected() : FALSE;

    if (!bWasConnected && m_bConnected)
    {
        m_uSessionUpdateFailedAt = 0;

        if (m_ptGameInfo)
        {
            // Re-register the game we were hosting when the connection dropped.
            ::new CreateSessionTask(LobbyService::Get(nPrimaryUserIndex), *m_ptGameInfo);

            m_uGameInfoUpdatedAt = 0;
            m_bGameInfoPlayersChanged = FALSE;
        }

        GetCountry();
    }

    if (m_bConnected &&
        m_ptGameInfo &&
        m_uGameInfoUpdatedAt &&
        Blizzard::Time::GetTimeElapsed(m_uGameInfoUpdatedAt, Blizzard::Time::Milliseconds()) > MATCHMAKING_UPDATE_GAME_DELAY_MS)
    {
        ::new UpdateSessionTask(LobbyService::Get(nPrimaryUserIndex), *m_ptGameInfo, m_bGameInfoPlayersChanged);

        m_uSessionUpdateFailedAt = 0;
        m_uGameInfoUpdatedAt = 0;

        m_bGameInfoPlayersChanged = FALSE;
    }
}








}  // namespace Online
}  // namespace Console
static void sSearchStubCallback(Console::Online::MMSearchResult eResult)
{
}
namespace Console {
namespace Online {
void MatchmakerInternal::Search(MatchParameters & tParams, SearchCallback pfnCallback)
{
    // Callers may not care about the result.
    if (!pfnCallback)
    {
        pfnCallback = sSearchStubCallback;
    }

    int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();

    if (nPrimaryUserIndex != -1)
    {
        m_ptSearchTask = ::new SearchTask(LobbyService::Get(nPrimaryUserIndex), tParams, pfnCallback);
    }
    else
    {
        pfnCallback(MMSEARCH_FAILED);
    }
}







BOOL MatchmakerInternal::IsSearching()
{
    if (!m_ptSearchTask)
        return FALSE;

    Task::Status eStatus = m_ptSearchTask->GetStatus();
    return eStatus != Task::TASK_DONE && eStatus != Task::TASK_FAILED;
}

// NON_MATCHING: 4 instructions; the loop latch tests i before uFound.






// NON_MATCHING: loop latch order: the original tests uFound < uMaxResults (line 649) before i++ / i < uNumResults (650); ours tests i first. Swapping the two conditions changes nothing.
uint32 MatchmakerInternal::GetSearchResults(
    MatchmakerResults * ptResults,
    uint32 uMaxResults)
{
    if (!m_ptSearchTask ||
        m_ptSearchTask->GetStatus() != Task::TASK_DONE)
    {
        return 0;
    }

    SearchTask * ptSearchTask = reference_cast<SearchTask>(m_ptSearchTask);
    uint32 uFound = 0;
    uint32 uNumResults = ptSearchTask->m_uNumResults;  // DWARF decl line is 647, but the original reads the count once, before the test (635:35)
    if (uNumResults == 0)
    {
        m_ptSearchTask = NULL;
        return uFound;
    }

    BOOL bRegionStrictMatching = Prefs::GetConsoleMultiplayerPrefs().bMatchmakeRegionStrict;
    BOOL bLanguageStrictMatching = Prefs::GetConsoleMultiplayerPrefs().bMatchmakeLanguageStrict;

    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();

    for (uint32 i = 0;
         uFound < uMaxResults && i < uNumResults;
         i++)
    {
        MatchMakingInfo & tSearchResult = ptSearchTask->m_tResults[i];
        MatchmakerResults & tResult = ptResults[uFound];


        if ((bRegionStrictMatching && (tSearchResult.m_memberRegion != ptSearchTask->m_tMatchParams.eRegion)) ||
            (bLanguageStrictMatching && (tSearchResult.m_memberLanguage != ptSearchTask->m_tMatchParams.nLanguage)))
        {
            continue;
        }

        bdCommonAddrRef ptAddr = tSearchResult.getHostAddrAsCommonAddr(me);

        tResult.ptAddress = ptAddr;
        tResult.tSecID = tSearchResult.m_memberSecurityID;
        tResult.tSecKey = tSearchResult.m_memberSecurityKey;
        tResult.tParams.eGameType = tSearchResult.m_memberMode == QUERY_QM_CHALLENGE_RIFT ? GT_CHALLENGE_RIFTS : GT_DIABLO3;
        tResult.tParams.bHardcore = tSearchResult.m_memberMode == QUERY_QM_HARDCORE || tSearchResult.m_memberMode == QUERY_QM_SEASONAL_HARDCORE;
        tResult.tParams.bSeasonal = tSearchResult.m_memberMode == QUERY_QM_SEASONAL || tSearchResult.m_memberMode == QUERY_QM_SEASONAL_HARDCORE;
        tResult.tParams.nLevel = tSearchResult.m_memberLevel;
        tResult.tParams.eAct = (Act)tSearchResult.m_memberAct;
        tResult.tParams.snoQuest = tSearchResult.m_memberQuest;
        tResult.tParams.uNumPlayers = tSearchResult.m_numPlayers;

        uFound++;
    }

    m_ptSearchTask = NULL;

    return uFound;
}




void MatchmakerInternal::Cancel()
{
    if (m_ptSearchTask)
    {
        m_ptSearchTask->Cancel();
        m_ptSearchTask = NULL;
    }
}







void MatchmakerInternal::RegisterGame(const bdSecurityID & tID, const bdSecurityKey & tKey, const MatchParameters & tParams)
{
    // Only register real games.
    if (tParams.nLevel > 0)
    {








        bdCommonAddrRef ptAddr = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();

        ::delete m_ptGameInfo; m_ptGameInfo = NULL;
        m_ptGameInfo = ::new MatchMakingInfo(ptAddr, tID, tKey);

        m_ptGameInfo->m_memberPublic = tParams.bIsPublic ? 1 : 0;
        m_ptGameInfo->m_numPlayers = tParams.uNumPlayers;
        m_ptGameInfo->m_memberOpenSlots = m_ptGameInfo->m_maxPlayers - m_ptGameInfo->m_numPlayers;
        m_ptGameInfo->m_memberMode = MatchParamsToQMMode(tParams);
        m_ptGameInfo->m_memberLevel = tParams.nLevel;
        m_ptGameInfo->m_memberAct = tParams.eAct;
        m_ptGameInfo->m_memberQuest = tParams.snoQuest;
        m_ptGameInfo->m_memberMonsterPower = tParams.nHandicap;
        m_ptGameInfo->m_memberLanguage = GetCurrentLanguage();
        m_ptGameInfo->m_memberRegion = GetCurrentRegion(tParams.eRegion);
        m_ptGameInfo->m_memberTag = tParams.nTag;
        m_ptGameInfo->m_memberReputation = tParams.bBadReputation ? 1 : 0;

        int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();

        if (nPrimaryUserIndex != -1)
        {
            // Create the session now if we can, otherwise connect first.
            if (m_bConnected)
            {
                ::new CreateSessionTask(LobbyService::Get(nPrimaryUserIndex), *m_ptGameInfo);
                m_uSessionUpdateFailedAt = 0;
            }
            else
            {
                LobbyService::Get(nPrimaryUserIndex)->Connect();
            }
        }
    }
}



void MatchmakerInternal::UnregisterGame()
{
    if (IsRegistered())
    {
        ::delete m_ptGameInfo;
        m_ptGameInfo = NULL;

        int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();
        if (nPrimaryUserIndex != -1 && m_bConnected)
        {
            ::new DeleteSessionTask(LobbyService::Get(nPrimaryUserIndex));
            m_uSessionUpdateFailedAt = 0;
        }
    }
}









void MatchmakerInternal::UpdateGame(const bdSecurityID & tID, const bdSecurityKey & tKey, const MatchParameters & tParams)
{
    if (tParams.nLevel != -1)
    {
        if (!IsRegistered())
        {
            // Retry registering, but not too often after a failure.




            if (m_uSessionUpdateFailedAt != 0)
            {
                if (Blizzard::Time::GetTimeElapsed(m_uSessionUpdateFailedAt, Blizzard::Time::Milliseconds()) < 60000)
                {
                    return;
                }
            }

            RegisterGame(tID, tKey, tParams);
        }
        else
        {
            if (bdMemcmp(&tID, &m_ptGameInfo->m_memberSecurityID, sizeof(bdSecurityID)) != 0 ||
                bdMemcmp(&tKey, &m_ptGameInfo->m_memberSecurityKey, sizeof(bdSecurityKey)) != 0)
            {
                // A different game; drop the old registration.
                UnregisterGame();
                return;
            }

            int32 nReputation = tParams.bBadReputation ? 1 : 0;
            int32 nMode = MatchParamsToQMMode(tParams);

            bdInt32 nPublic = tParams.bIsPublic ? 1 : 0;

            if (m_ptGameInfo->m_memberPublic == nPublic &&
                m_ptGameInfo->m_memberMode == nMode &&
                m_ptGameInfo->m_memberLevel == tParams.nLevel &&
                m_ptGameInfo->m_memberAct == tParams.eAct &&
                m_ptGameInfo->m_memberQuest == tParams.snoQuest &&
                m_ptGameInfo->m_memberMonsterPower == tParams.nHandicap &&
                m_ptGameInfo->m_memberTag == tParams.nTag &&
                m_ptGameInfo->m_memberReputation == nReputation &&
                m_ptGameInfo->m_numPlayers == tParams.uNumPlayers &&
                m_ptGameInfo->m_memberRegion == GetCurrentRegion(tParams.eRegion))
            {
                return;
            }





            BOOL bNumPlayersChanged = m_ptGameInfo->m_numPlayers != tParams.uNumPlayers;

            m_ptGameInfo->m_memberPublic = nPublic;
            m_ptGameInfo->m_memberMode = nMode;
            m_ptGameInfo->m_memberLevel = tParams.nLevel;
            m_ptGameInfo->m_memberAct = tParams.eAct;
            m_ptGameInfo->m_memberQuest = tParams.snoQuest;
            m_ptGameInfo->m_memberMonsterPower = tParams.nHandicap;
            m_ptGameInfo->m_memberTag = tParams.nTag;
            m_ptGameInfo->m_memberReputation = nReputation;
            m_ptGameInfo->m_numPlayers = tParams.uNumPlayers;
            m_ptGameInfo->m_memberOpenSlots = m_ptGameInfo->m_maxPlayers - m_ptGameInfo->m_numPlayers;


            m_ptGameInfo->m_memberRegion = GetCurrentRegion(tParams.eRegion);

            // The session itself is updated from Idle.
            int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();
            if (nPrimaryUserIndex != -1 && m_bConnected)
            {
                m_uGameInfoUpdatedAt = Blizzard::Time::Milliseconds();
                m_bGameInfoPlayersChanged = m_bGameInfoPlayersChanged || bNumPlayersChanged;
            }
        }
    }
}



MatchmakerRegion MatchmakerInternal::GetCurrentRegion(MatchmakerRegion eRegionPreference)
{
    static const struct
    {
        char szCountry[4];
        MatchmakerRegion eRegion;
    } satCountryToRegion[] = {
        { "us", REGION_NORTH_AMERICA },
        { "ca", REGION_NORTH_AMERICA },
        { "mx", REGION_NORTH_AMERICA },
        { "ar", REGION_SOUTH_AMERICA },
        { "br", REGION_SOUTH_AMERICA },
        { "cl", REGION_SOUTH_AMERICA },
        { "co", REGION_SOUTH_AMERICA },
        { "pe", REGION_SOUTH_AMERICA },
        { "ae", REGION_EUROPE },
        { "at", REGION_EUROPE },
        { "be", REGION_EUROPE },
        { "bg", REGION_EUROPE },
        { "bh", REGION_EUROPE },
        { "ch", REGION_EUROPE },
        { "cy", REGION_EUROPE },
        { "cz", REGION_EUROPE },
        { "de", REGION_EUROPE },
        { "dk", REGION_EUROPE },
        { "es", REGION_EUROPE },
        { "fi", REGION_EUROPE },
        { "fr", REGION_EUROPE },
        { "gb", REGION_EUROPE },
        { "gr", REGION_EUROPE },
        { "hr", REGION_EUROPE },
        { "hu", REGION_EUROPE },
        { "ie", REGION_EUROPE },
        { "il", REGION_EUROPE },
        { "is", REGION_EUROPE },
        { "it", REGION_EUROPE },
        { "kw", REGION_EUROPE },
        { "lb", REGION_EUROPE },
        { "lu", REGION_EUROPE },
        { "mt", REGION_EUROPE },
        { "nl", REGION_EUROPE },
        { "no", REGION_EUROPE },
        { "om", REGION_EUROPE },
        { "pl", REGION_EUROPE },
        { "pt", REGION_EUROPE },
        { "qa", REGION_EUROPE },
        { "ro", REGION_EUROPE },
        { "sa", REGION_EUROPE },
        { "se", REGION_EUROPE },
        { "si", REGION_EUROPE },
        { "sk", REGION_EUROPE },
        { "tr", REGION_EUROPE },
        { "ru", REGION_RUSSIA },
        { "ua", REGION_RUSSIA },
        { "hk", REGION_ASIA },
        { "in", REGION_ASIA },
        { "jp", REGION_ASIA },
        { "kr", REGION_ASIA },
        { "tw", REGION_ASIA },
        { "au", REGION_AUSTRALIA },
        { "id", REGION_AUSTRALIA },
        { "my", REGION_AUSTRALIA },
        { "nz", REGION_AUSTRALIA },
        { "sg", REGION_AUSTRALIA },
        { "th", REGION_AUSTRALIA },
        { "za", REGION_AFRICA },
    };

    if (eRegionPreference != REGION_DEFAULT)
        return eRegionPreference;

    const char * szCountry = m_szCountryCode;
    for (int32 i = 0; i < sizeof(satCountryToRegion) / sizeof(satCountryToRegion[0]); i++)
    {
        if (strncasecmp(satCountryToRegion[i].szCountry, szCountry, 2) == 0)
            return satCountryToRegion[i].eRegion;
    }

    return REGION_DEFAULT;
}






















































void MatchmakerInternal::GetCountry()
{
    int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();
    if (nPrimaryUserIndex != -1)
    {
        ::new GetDMLUserInfoTask(LobbyService::Get(nPrimaryUserIndex), DMLCallback(OnlineService::internal::method_bind_0_1<void, MatchmakerInternal, MatchmakerInternal *, const bdDMLInfo &, &MatchmakerInternal::OnGotCountry>(this)));
    }
}





inline void MatchmakerInternal::OnGotCountry(const bdDMLInfo & tInfo)
{
    Blizzard::String::Copy(m_szCountryCode, tInfo.m_countryCode, sizeof(m_szCountryCode));
}







inline int32 MatchmakerInternal::GetCurrentLanguage()
{
    // The console's system language decides which games a search finds.
    return Console::Utilities::GetSystemLanguage();
}




void CreateSessionTask::Start()
{
    m_ptRemoteTask = m_ptLobbyService->GetMatchMaking()->createSession(&m_tInfo, &m_idSession);
}






void CreateSessionTask::OnComplete()
{
    sptMatchmaker->m_idSession = m_idSession;
}






void CreateSessionTask::OnFailed()
{
    if (m_ptLobbyService->IsConnected())
    {
        sptMatchmaker->OnFailSessionUpdate();
    }
}










void UpdateSessionTask::Start()
{
    if (m_bUpdatePlayers)
    {
        m_ptRemoteTask = m_ptLobbyService->GetMatchMaking()->updateSessionPlayers(sptMatchmaker->m_idSession, &m_tInfo, m_tInfo.m_numPlayers);
    }
    else
    {
        m_ptRemoteTask = m_ptLobbyService->GetMatchMaking()->updateSession(sptMatchmaker->m_idSession, &m_tInfo);
    }
}












void UpdateSessionTask::OnFailed()
{
    if (m_ptLobbyService->IsConnected() && m_ptRemoteTask->getErrorCode() == BD_NO_ENTRY_TO_UPDATE)
    {
        sptMatchmaker->OnFailSessionUpdate();
    }
}














void DeleteSessionTask::Start()
{
    m_ptRemoteTask = m_ptLobbyService->GetMatchMaking()->deleteSession(sptMatchmaker->m_idSession);
}






void DeleteSessionTask::OnComplete()
{
}






void DeleteSessionTask::OnFailed()
{
    m_ptLobbyService->IsConnected();
}












void SearchTask::Start()
{
    m_tMatchParams.nLanguage = sptMatchmaker->GetCurrentLanguage();
    m_tMatchParams.eRegion = sptMatchmaker->GetCurrentRegion((MatchmakerRegion)Prefs::GetConsoleMultiplayerPrefs().nMatchmakeRegion);

    m_ptQuery = ::new QuickMatchQuery();
    QuickMatchQuery * ptMMQuery = static_cast<QuickMatchQuery *>(m_ptQuery);

    ptMMQuery->m_memberparam_Mode = MatchParamsToQMMode(m_tMatchParams);
    int32 nLevel = m_tMatchParams.nLevel;
    ptMMQuery->m_memberparam_Level = nLevel;
    ptMMQuery->m_memberparam_Act = m_tMatchParams.eAct;
    ptMMQuery->m_memberparam_Quest = m_tMatchParams.snoQuest;
    ptMMQuery->m_memberparam_MonsterPower = m_tMatchParams.nHandicap;
    ptMMQuery->m_memberparam_NumSlots = m_tMatchParams.uNumPlayers;
    ptMMQuery->m_memberparam_Language = m_tMatchParams.nLanguage;
    ptMMQuery->m_memberparam_Region = m_tMatchParams.eRegion;
    ptMMQuery->m_memberparam_Tag = m_tMatchParams.nTag;
    ptMMQuery->m_memberparam_Reputation = m_tMatchParams.bBadReputation ? REPUTATION_BAD : REPUTATION_GOOD;

    // Search within a level range that widens with the square root of the level.
    int32 nMaxLevelDifference = Math::Max(Math::FloatRoundToInt(Math::Sqrt((float)Math::Max(nLevel / 2, 1))), 2);
    ptMMQuery->m_memberparam_LevelMin = nLevel - nMaxLevelDifference;
    ptMMQuery->m_memberparam_LevelMax = nLevel + nMaxLevelDifference;

    QuickMatchQueryType eQuery;
    if (m_tMatchParams.eGameType == GT_CHALLENGE_RIFTS)
        eQuery = QUERY_QM_QUEST;
    else
    {
        if (m_tMatchParams.eAct == ACT_INVALID)
            eQuery = QUERY_QM_ANY;
        else if (m_tMatchParams.eAct == ACT_OPEN_WORLD || m_tMatchParams.snoQuest == -1)
            eQuery = QUERY_QM_ACT;
        else
            eQuery = QUERY_QM_QUEST;
    }

    m_ptRemoteTask = m_ptLobbyService->GetMatchMaking()->findSessions(eQuery, 0, 20,
                                                                      m_ptQuery,
                                                                      m_tResults);
}











void SearchTask::OnComplete()
{
    m_uNumResults = m_ptRemoteTask->getNumResults();
    m_pfnCallback(MMSEARCH_SUCCESS);
}






void SearchTask::OnFailed()
{
    if (!m_ptLobbyService->IsConnected())
    {
        m_pfnCallback(MMSEARCH_DISCONNECTED);
    }
    else
    {
        m_pfnCallback(MMSEARCH_FAILED);
    }

    sptMatchmaker->m_ptSearchTask = NULL;
}







void SearchTask::OnCancel()
{
    m_pfnCallback(MMSEARCH_CANCELLED);
}






void GetDMLUserInfoTask::Start()
{
    m_ptRemoteTask = m_ptLobbyService->GetDML()->getUserData(&m_tUserData);
}





void GetDMLUserInfoTask::OnComplete()
{
    m_pfnCallback(m_tUserData);
}







void GetDMLUserInfoTask::OnFailed()
{
}













bdBool QuickMatchQuery::serialize(bdByteBuffer & buffer) const
{
    bdBool ok = bdSessionParams::serialize(buffer);
    ok = ok && buffer.writeInt32(m_memberparam_Level);
    ok = ok && buffer.writeInt32(m_memberparam_LevelMin);
    ok = ok && buffer.writeInt32(m_memberparam_LevelMax);
    ok = ok && buffer.writeInt32(m_memberparam_Act);
    ok = ok && buffer.writeInt32(m_memberparam_Quest);
    ok = ok && buffer.writeInt32(m_memberparam_Mode);
    ok = ok && buffer.writeUInt32(m_memberparam_NumSlots);
    ok = ok && buffer.writeInt32(m_memberparam_Language);
    ok = ok && buffer.writeInt32(m_memberparam_Region);
    ok = ok && buffer.writeInt32(m_memberparam_MonsterPower);
    ok = ok && buffer.writeInt32(m_memberparam_Tag);
    ok = ok && buffer.writeInt32(m_memberparam_Reputation);
    return ok;
}





bdBool MatchMakingInfo::serialize(bdByteBuffer & buffer) const
{
    bdBool ok = bdMatchMakingInfo::serialize(buffer);
    ok = ok && buffer.writeInt32(m_memberLevel);
    ok = ok && buffer.writeInt32(m_memberAct);
    ok = ok && buffer.writeInt32(m_memberQuest);
    ok = ok && buffer.writeBlob(&m_memberSecurityID, sizeof(bdSecurityID));
    ok = ok && buffer.writeBlob(&m_memberSecurityKey, sizeof(bdSecurityKey));
    ok = ok && buffer.writeInt32(m_memberMode);
    ok = ok && buffer.writeUInt32(m_memberOpenSlots);
    ok = ok && buffer.writeInt32(m_memberLanguage);
    ok = ok && buffer.writeInt32(m_memberRegion);
    ok = ok && buffer.writeInt32(m_memberMonsterPower);
    ok = ok && buffer.writeInt32(m_memberTag);
    ok = ok && buffer.writeInt32(m_memberPublic);
    ok = ok && buffer.writeInt32(m_memberReputation);
    return ok;
}





bdBool MatchMakingInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = bdMatchMakingInfo::deserialize(buffer);
    ok = ok && buffer->readInt32(m_memberLevel);
    ok = ok && buffer->readInt32(m_memberAct);
    ok = ok && buffer->readInt32(m_memberQuest);
    bdUInt32 tempBlobLength_SecurityID = sizeof(bdSecurityID);
    ok = ok && buffer->readBlob(m_memberSecurityID.ab, tempBlobLength_SecurityID);
    bdUInt32 tempBlobLength_SecurityKey = sizeof(bdSecurityKey);
    ok = ok && buffer->readBlob(m_memberSecurityKey.ab, tempBlobLength_SecurityKey);
    ok = ok && buffer->readInt32(m_memberMode);
    ok = ok && buffer->readUInt32(m_memberOpenSlots);
    ok = ok && buffer->readInt32(m_memberLanguage);
    ok = ok && buffer->readInt32(m_memberRegion);
    ok = ok && buffer->readInt32(m_memberMonsterPower);
    ok = ok && buffer->readInt32(m_memberTag);
    ok = ok && buffer->readInt32(m_memberPublic);
    ok = ok && buffer->readInt32(m_memberReputation);
    return ok;
}

}  // namespace Online
}  // namespace Console
