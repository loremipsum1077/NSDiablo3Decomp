#include "../../Common/SNO/SNOName.h"  // X/Client/Console/LobbyService.cpp. SNOName.h/WorldPlace.h/GfxVB.h first: the original static initializer sets SNONAME_NONE, cgplaceNull and cgc*Normal before the nn::ldn statics (DWARF variable order agrees); the original include chain is not known.
#include "../../Common/WorldPlace.h"  // The DemonWare lobby connection for each local user on the console builds, its remote tasks
#include "../Gfx/GfxVB.h"  // (storage, leaderboards, account lookups) and the config, seasons and blacklist files it reads.
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdCounter/bdCounter.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdCounter/bdCounterValue.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccounts.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccountsResult.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyEventHandler.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceData.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdStats/bdStatsInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdContextUserStorageFileInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdFileData.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdStorage.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTimeStamp.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdUserInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyService.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMessaging/bdMessaging.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTitleUtilities.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTask.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../../Contrib/Contrib/blz/include/blz/embedded_string.h"
#include "../../../Contrib/Contrib/blz/include/blz/map.h"
#include "../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/Leaderboard.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/Player.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Protobuf/Leaderboard.pb.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "GamerProfile.h"
#include "PresenceManager.h"
#include "LobbyService.h"
#include "Online.h"
#include "TaskManager.h"

// From Contrib/Contrib/DemonWare/bdLobby/bdAuth/bdAuthSwitch.h (not in src/ yet): only its
// virtual destructor is used here.
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdAuth/bdAuthSwitch.h"





namespace Console {
namespace Online {
// Defined in X/Client/Console/Online.cpp.
extern NetworkMode seNetworkMode;
extern BOOL sbNetworkThreadDone;
void sHandleMultipleLogon(const int32 nUserIndex);
void QueueNetworkEventForMainThread(OnlineService::Callback<void ()> pfnEvent); void DeferNetworkEventForMainThread(uint32 uDelayMS, OnlineService::Callback<void ()> pfnEvent);
void QueueNetworkEventForMainThreadAlways(OnlineService::Callback<void ()> pfnEvent);
}  // namespace Online
}  // namespace Console

// Defined in X/Common/Leaderboard.cpp.
blz::string LeaderboardCompressMessage(const google::protobuf::Message & tMessage, int nLevel);
bool LeaderboardDecompressMessage(const blz::string & szData, google::protobuf::Message * ptMessage);
bdLeaderBoardID LeaderboardToDemonwareLeaderboardId(LeaderboardId idLeaderboard, LeaderboardScope eScope, uint32 uPartition, bool bMetadata); bool LeaderboardIsChallengeRift(uint64_t leaderboardId); uint64 LeaderboardMakeAchievementPointScore(uint32 achievementPoints, uint32 unixTime); int64 LeaderboardMakeAchievementDemonwareRating(uint64 score); uint64 LeaderboardMakeConquestScore(Blizzard::Time::Timestamp ts); int64 LeaderboardMakeConquestDemonwareRating(uint64 score); class Player; Player * PlayerGetByControllerIndex(int32 nControllerIndex); namespace SLeaderboard { void FillInMemberMetadata(D3::Leaderboard::Member * ptMember, const Player * ptPlayer, bool bArg); } bool LeaderboardIsRift(uint64 leaderboardId); int64 LeaderboardMakeRiftDemonwareRating(uint64 score); uint32 LeaderboardGetRiftScoreLevel(uint64 score); Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score); uint64 LeaderboardMakeRiftScore(uint32 level, int32 millisecondsElapsed, uint32 unixTime); void SPlayerSetPerkBuffs(float, float, float, int, int, int, int, int); extern XVarUint32 s_varSeasonNum; extern XVarUint32 s_varSeasonState; extern XVarString s_varSeasonShutdownDate;

#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
SNOGroup SNOGroupGetByName(LPCSTR szName);  // X/Common/SNO/SNOGroups.cpp:156
// X/Server/OnlineService/OnlineServiceServerPersist.h:355, 365 (not in src/ yet).
struct SNONameHasher {
    size_t operator()(const SNOName & name) const { return Blizzard::Util::Hash32(&name, sizeof(name)); }
};
struct SNONameEquals {
    bool operator()(const SNOName & left, const SNOName & right) const { return right == left; }
};

// Original initializer calls and host strings identify the three environment arguments.
bdEnvironmentString stAuthEnvironmentStrings("crimson-switch-auth3.dev.demonware.net", "crimson-switch-auth3.cert.demonware.net", "crimson-switch-auth3.prod.demonware.net");



// Original initializer calls and host strings identify the three environment arguments.
bdEnvironmentString stLobbyEnvironmentStrings("crimson-switch-lobby.dev.demonware.net", "crimson-switch-lobby.cert.demonware.net", "crimson-switch-lobby.prod.demonware.net");


#include "../Definitions/PreferencesDefinition.h"
#include "../../Sigma/UnicodeText.h"
// DWARF X/Common/Console/TitleVersion.h:25; TitleVersion enumerator value 265.
namespace Console { enum TitleVersion { TITLE_VERSION_CURRENT = 277 }; }
#include "MailService.h"
// DWARF HeroPublishing.cpp:129, SaveMigration.cpp:1601, UpdateManager.cpp:381/417.
namespace Console { namespace HeroPublishing { void SetPublishIntervalInMinutes(int32); } namespace SaveMigration { void SetCrossPlatformSaveMigrationEnabled(BOOL); } namespace UpdateManager { int32 GetCurrentUpdateVersion(XLocale &); void RemoveUpdates(); } }
// DWARF GamerProfile.h:258, GamerProfileNX64.cpp:718/910.
namespace Console { namespace GamerProfile { class ScopedUsersReadLock { public: ScopedUsersReadLock(); ~ScopedUsersReadLock(); }; nn::account::UserHandle GetUserHandle(int32); nn::account::Nickname GetUserNickName(int32); } }
// DWARF GamerProfileNX64.cpp:964/951; platform-account availability and subscription status.
namespace Console { namespace GamerProfile { BOOL IsEnsuringAccountAvailability(int32); void UpdateHasNSOSubscription(int32, BOOL); } }
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"










static const Blizzard::Time::Millisecond s_uInitialReconnectDelayMS = 1000;
static const float s_fReconnectEaseoffMultiplier = 2.0f;
static const Blizzard::Time::Millisecond s_uMaxReconnectDelayMS = 60000;


static const Blizzard::Time::Millisecond s_uNX64TokenFailDelay = 30 * 60 * 1000;


















namespace Console {
namespace Online {

class LobbyServiceInternal : public LobbyService, public bdLobbyEventHandler
{
public:
    LobbyServiceInternal(int32 nUserIndex, const bdTitleID idTitle);
    virtual ~LobbyServiceInternal();
    void IdleInternal();
    virtual void Connect();
    virtual void Disconnect();












    virtual BOOL IsActive()
    {
        // Resolving, authenticating, connecting and connected all count as active.
        return m_eConnectionState != CONNSTATE_INACTIVE &&
            m_eConnectionState != CONNSTATE_DISCONNECTED;
    }

    virtual BOOL IsConnected()
    {
        return m_eConnectionState == CONNSTATE_CONNECTED;
    }


    virtual BOOL IsConnecting()
    {
        return m_eConnectionState > CONNSTATE_INACTIVE && m_eConnectionState < CONNSTATE_CONNECTED;
    }



    virtual bdOnlineUserID GetOnlineUserID()
    {
        return m_idOnlineUser;
    }


    virtual TaskManager & GetTaskManager()
    {
        return m_tTaskManager;
    }


    virtual bdLobbyService * GetLobbyService()
    {
        return m_ptLobbyService;
    }


    virtual bdMatchMaking * GetMatchMaking()
    {
        // The default context.
        return m_ptLobbyService->getMatchMaking("");
    }


    virtual bdStorage * GetStorage()
    {
        // The default context.
        return m_ptLobbyService->getStorage("");
    }


    virtual bdStorage * GetStorage(const char * szContext, bdUserID & idUser)
    {
        // Storage in a named context is owned by the platform account.


        idUser = bdUserID(m_idOnlineUser, "nintendo");

        return m_ptLobbyService->getStorage(szContext);
    }


    virtual bdMail * GetMail()
    {
        // The default context.
        return m_ptLobbyService->getMail("");
    }


    virtual bdStats * GetStats()
    {
        // (236-238)
        return m_ptLobbyService->getStats();
    }


    virtual bdEventLog * GetEventLog()
    {
        // (243-245)
        return m_ptLobbyService->getEventLog();
    }



    virtual bdMessaging * GetMessaging()
    {
        // (251-253)
        return m_ptLobbyService->getMessaging();
    }


    virtual bdRichPresenceService * GetRichPresence()
    {
        // The default context.
        return m_ptLobbyService->getRichPresence("");
    }


    virtual bdDML * GetDML()
    {
        // (265-267)
        return m_ptLobbyService->getDML();
    }

    virtual void StorageGetPublisherFile(const char * szFilename, StorageGetFileCallback pfnCallback, bool bStartImmediately);
    virtual void StorageFindUserFile(bdUserAccountID idUser, const char * szContext, const char * szFilename, StorageFindUserFileCallback pfnCallback);
    virtual void StorageGetUserFile(const char * szContext, const bdUserStorageFileInfo & tFileInfo, StorageGetFileCallback pfnCallback);
    virtual void StoragePublishUserFile(const char * szContext, const char * szFilename, blz::string szData, StoragePublishUserFileCallback pfnCallback);
    virtual void LeaderboardWriteStats(blz::vector<LeaderboardStatsInfo, blz::allocator<LeaderboardStatsInfo> > & atStats, LeaderboardPostCallback pfnCallback);
    virtual void LeaderboardReadStatsByLeaderboardIds(const blz::vector<LeaderboardKeyAndPartition, blz::allocator<LeaderboardKeyAndPartition> > & idLeaderboards, LeaderboardReadStatsCallback pfnCallback);
    virtual void LeaderboardReadStatsByPivot(const LeaderboardKeyAndPartition & idLeaderboard, uint32 uMaxResults, LeaderboardReadStatsCallback pfnCallback);
    virtual void LeaderboardReadStatsByEntityId(const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<unsigned long, blz::allocator<unsigned long> > & aidUsers, bool bIncludeSelf, LeaderboardReadStatsCallback pfnCallback);
    virtual void LeaderboardReadStatsByRank(const LeaderboardKeyAndPartition & idLeaderboard, uint32 uFirstRank, uint32 uMaxResults, LeaderboardReadStatsCallback pfnCallback);
    virtual void LeaderboardReadMetadata(const LeaderboardKeyAndPartition & idLeaderboard, bdOnlineUserID idUser, LeaderboardReadStatsCallback pfnCallback);
    virtual void LookupUserAccounts(OnlineUserId * pidUsers, uint32 uNumUsers, LookupUserAccountsCallback pfnCallback);
    virtual void UpdateGlobalCounters(const CounterVector & atCounters);
    virtual void GetServerTime(GetServerTimeCallback pfnCallback, bool bStartImmediately);
    virtual BOOL SendGlobalInstantMessage(OnlineUserId idRecipient, const void * const ptMessage, const bdUInt uMsgSize);
    void RunTokenAuthentication_Internal();
    void VerifyNSOSubscriptionAsync_Internal(VerifyNSOCallback pfnCallback);



























































    void AttemptToConnectOnNX64() { m_bAttemptConnectForNX64 = TRUE; }




    virtual ConnectEvent & GetConnectEvent()
    {
        // (352-353)
        return m_tConnectEvent;
    }

    virtual DisconnectEvent & GetDisconnectEvent()
    {
        // (358-359)
        return m_tDisconnectEvent;
    }

    virtual NewMailEvent & GetNewMailEvent()
    {
        // (364-365)
        return m_tNewMailEvent;
    }

    class UpdateFileInfo;
    void OnConfigFileRetrieved(StorageResult eResult, blz::shared_ptr<blz::string> pszFileData);
    void OnSeasonsFileRetrieved(StorageResult eResult, blz::shared_ptr<blz::string> pszFileData);
    void OnBlacklistFileRetrieved(StorageResult eResult, blz::shared_ptr<blz::string> pszFileData);
    void OnUpdateInfoRetrieved(StorageResult eResult, blz::shared_ptr<blz::string> pszFileData);
    void OnUpdateFragmentRetrieved(UpdateFileInfo * ptUpdateInfo, uint32 uFragmentNum, StorageResult eResult, blz::shared_ptr<blz::string> pszFileData);
    static void OnServerTime(BOOL bSuccess, const bdTimeStamp & tTimestamp);

private:
    virtual void onNewMail();
    virtual void onSubscribedRichPresenceUpdate(const bdNChar8 * szContext, const bdTitleID idTitle, const bdRichPresenceSource tSource, const bdRichPresenceData & tData);
    virtual void onGlobalInstantMessage(bdOnlineUserID idSender, bdNChar8 * szSenderName, void * ptMessage, bdUInt uMessageSize);
    virtual void onMultipleLogon(bdOnlineUserID userID);














































    bool m_bFatalErrorOccurred;

    void OnDisconnect();


    void SetReconnectDelay()
    {

        m_uReconnectDelayMS = Math::Min((Blizzard::Time::Millisecond)(m_uReconnectDelayMS * s_fReconnectEaseoffMultiplier), s_uMaxReconnectDelayMS);

        m_bReconnect = TRUE;
    }


    void SetCustomReconnectDelay(
        Blizzard::Time::Millisecond uCustomReconnectDelay)
    {
        m_uReconnectDelayMS = uCustomReconnectDelay;
        // Inference: shared line-zero tail sets reconnect after assigning the custom delay.
        m_bReconnect = TRUE;
    }








    enum ConnectionState
    {
        CONNSTATE_INACTIVE,
        CONNSTATE_RESOLVE,
        CONNSTATE_RESOLVE_PENDING,
        CONNSTATE_AUTH_START,
        CONNSTATE_AUTH_PENDING,
        CONNSTATE_CONNECT,
        CONNSTATE_CONNECTING,
        CONNSTATE_CONNECTED,
        CONNSTATE_DISCONNECTING,
        CONNSTATE_DISCONNECTED,
    };

    int32 m_nUserIndex;

    bdTitleID m_idTitle;
    ConnectionState m_eConnectionState;
    bool m_bWasConnected;

    bdLobbyService * m_ptLobbyService;





    bdAuthSwitch * m_ptAuthService;
    bdNChar8 m_szNickNameUsedForAuth[32];




    ConnectEvent m_tConnectEvent;
    DisconnectEvent m_tDisconnectEvent;
    NewMailEvent m_tNewMailEvent;


    BOOL m_bRunningTokenAuthentication;
    BOOL m_bAttemptConnectForNX64;
    VerifyNSOCallback m_pfnVerifyNSOCallback;

    BOOL m_bReconnect;
    BOOL m_bReconnectWhenSignedIn;
    Blizzard::Time::Millisecond m_uLastReconnectAttemptMS;
    Blizzard::Time::Millisecond m_uReconnectDelayMS;

    bdOnlineUserID m_idOnlineUser;

    BOOL m_bCheckedForBnetAccount;


    int32 m_nBalanceVersion;
public:
    class UpdateFileInfo
    {
    public:
        UpdateFileInfo()
            : m_uFileSize(0)
        {
        }

        void Clear()
        {
            m_szFilename.clear();
            m_szTOCFilename.clear();

            m_uFileSize = 0;
            blz::string().swap(m_szBuffer);
        }

        blz::embedded_string<128> m_szFilename;
        blz::embedded_string<128> m_szTOCFilename;
        uint32 m_uFileSize;
        blz::string m_szBuffer;
    };

    enum FileType
    {
        FILE_NONE = -1,
        FILE_UPDATES_GLOBAL,
        FILE_UPDATES_LOCALE,
        NUM_FILE_TYPES,
    };


    UpdateFileInfo m_atUpdateFiles[NUM_FILE_TYPES];
    int32 m_nFileInProgress;

    TaskManager m_tTaskManager;

    static int32 ms_nNumLobbyServicesConnected;
};







class LeaderboardStatsInfo : public bdStatsInfo
{
public:
    LeaderboardStatsInfo() : m_score(0), m_timestamp(0) {}

    virtual bdBool serialize(bdByteBuffer & buffer) const {
        bdBool ok = bdStatsInfo::serialize(buffer);

        ok = ok && buffer.writeUInt64(m_score);
        ok = ok && buffer.writeInt64(m_timestamp);

        blz::string blob = LeaderboardCompressMessage(m_metadata, 9);
        ok = ok && buffer.writeBlob(blob.c_str(), blob.length());
        return ok;
    }

    virtual bdBool deserialize(bdByteBufferRef buffer)
    {
        bdBool ok = bdStatsInfo::deserialize(buffer);
        ok = ok && buffer->readUInt64(m_score);
        ok = ok && buffer->readInt64(m_timestamp);

        bdUInt32 uLength = 8192;
        blz::string szBuffer;
        ok = ok && buffer->readBlob((bdUByte8 *)szBuffer.modify_data(uLength), uLength);
        if (ok)
        {
            szBuffer.set_size(uLength);
            ok = LeaderboardDecompressMessage(szBuffer, &m_metadata);
        }

        return ok;
    }

    virtual bdUInt sizeOf()
    {
        return sizeof(LeaderboardStatsInfo);
    }

    bdUInt64 m_score;
    bdInt64 m_timestamp;
    D3::Leaderboard::Metadata m_metadata;

    LeaderboardKeyAndPartition m_lbKey;
};

int32 LobbyServiceInternal::ms_nNumLobbyServicesConnected = 0;

LobbyServiceInternal * sptLobby[4];

bool sbBlacklistDownloadPending;
bool sbBlacklistDataReceived;
typedef int32 GBID; typedef blz::unordered_map<GBID, uint32> GBIDBlacklistMap; GBIDBlacklistMap tBlacklistedGBIDs;
typedef blz::unordered_map<SNOName, uint32, SNONameHasher, SNONameEquals> SNOBlacklistMap; SNOBlacklistMap tBlacklistedSNOs;

BOOL sbHoldTasksForConfig;

XOptional<long> stTimeOffset;
Blizzard::Time::Timestamp suLastTimeCheck;
bool sbTimeCheckInProgress;
Blizzard::Time::Timestamp GetNetworkTimestamp() { return Blizzard::Time::GetTimestamp() + (stTimeOffset.Exists() ? stTimeOffset.Get() : 0); }

BOOL sbCommunityBuffEnabled;
Blizzard::Time::Timestamp stCommunityBuffStartTime;
Blizzard::Time::Timestamp stCommunityBuffEndTime;
float sflXPBonus;
float sflGoldFindBonus;
float sflLegendaryFindBonus;
BOOL sbDoubleTreasureGoblins;
BOOL sbDoubleBloodShards;
BOOL sbDoubleRiftKeystones;
BOOL sbEasterEggWorldEnabled;
BOOL sbDoubleBountyBags;
BOOL sbSeasonalGlobalLeaderboardsEnabled;
void sEvaluateSeasonState(bool bAllowCorrectiveAction); SeasonsChangedEvent & GetSeasonsChangedEvent();
struct SeasonInfo
{
    SeasonInfo() : uSeasonNum(0), tPreSeasonStartTime(0), tStartTime(0), tEndTime(0) {}

    uint32 uSeasonNum;
    Blizzard::Time::Timestamp tPreSeasonStartTime;
    Blizzard::Time::Timestamp tStartTime;
    Blizzard::Time::Timestamp tEndTime;
};
bool sbSeasonsDataReceived;
blz::vector<SeasonInfo> satSeasons;
blz::atomic_bool sbSeasonsInitialized(false);
bool sbSeasonsDownloadPending;
Blizzard::Time::Timestamp suSeasonEndTime;














BOOL IsSeasonsInitialized()
{
    // (662)
    return sbSeasonsInitialized;
}




void sEvaluateSeasonState(bool bAllowCorrectiveAction)
{
    // (672-685)













    if (!sbSeasonsDataReceived ||
        !stTimeOffset.Exists() ||
        sbTimeCheckInProgress)
    {
        return;
    }

    Blizzard::Time::Timestamp nowNetwork = GetNetworkTimestamp();

    uint32 uExpectedSeason = 0;
    OnlineService::SeasonState eExpectedSeasonState = OnlineService::SEASON_STATE_ENDED;
    Blizzard::Time::Timestamp uExpectedSeasonEndTime = 0;

    // (699)
    for (const SeasonInfo & tSeasonInfo : satSeasons)
    {
        if (tSeasonInfo.tEndTime != 0 && nowNetwork >= tSeasonInfo.tEndTime)
        {
            eExpectedSeasonState = OnlineService::SEASON_STATE_ENDED;
        }
        else if (nowNetwork >= tSeasonInfo.tStartTime)
        {
            eExpectedSeasonState = OnlineService::SEASON_STATE_ACTIVE;
        }
        else if (tSeasonInfo.tPreSeasonStartTime != 0 && nowNetwork >= tSeasonInfo.tPreSeasonStartTime)
        {
            eExpectedSeasonState = OnlineService::SEASON_STATE_PRE;
        }
        else
        {
            break;
        }

        uExpectedSeason = tSeasonInfo.uSeasonNum;
        uExpectedSeasonEndTime = tSeasonInfo.tEndTime;
    }

    // (721-731)
    if ((uExpectedSeason < s_varSeasonNum.Get() ||
         (uExpectedSeason == s_varSeasonNum.Get() && eExpectedSeasonState < (OnlineService::SeasonState)s_varSeasonState.Get())) &&
        !bAllowCorrectiveAction)
    {
        uExpectedSeason = s_varSeasonNum.Get();
        eExpectedSeasonState = (OnlineService::SeasonState)s_varSeasonState.Get();
    }

    if (uExpectedSeason == s_varSeasonNum.Get() &&
        eExpectedSeasonState == (OnlineService::SeasonState)s_varSeasonState.Get())
    {
        sbSeasonsInitialized = true;
    }
    else
    {
        s_varSeasonNum.Set(uExpectedSeason, XVarBase::DIRTY_DEFAULT);
        s_varSeasonState.Set(eExpectedSeasonState, XVarBase::DIRTY_DEFAULT);
        sbSeasonsInitialized = true;
        QueueNotification(GetSeasonsChangedEvent());  // Online.h:1375, inlined at 752 per DWARF
    }

    // (758)
    if (uExpectedSeasonEndTime != suSeasonEndTime)
    {
        suSeasonEndTime = uExpectedSeasonEndTime;

        if (suSeasonEndTime != 0)
        {
            // (765-766)
            Blizzard::Time::TimeRec tr;
            Blizzard::Time::BreakTime(suSeasonEndTime, tr);

            blz::string szTime = blz::make_stringf("%04u-%02u-%02uT%02u:%02u:%02u.000Z", tr.year, tr.month, tr.day, tr.hour, tr.minute, tr.second);
            s_varSeasonShutdownDate.Set(szTime.c_str(), XVarBase::DIRTY_DEFAULT);
        }
        else
        {
            s_varSeasonShutdownDate.Set("", XVarBase::DIRTY_DEFAULT);
        }
    }
}
















uint32 GetSecondsUntilSeasonEnd()
{
    if (OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE)
    {
        return 0;
    }

    // (786-787)

    if (suSeasonEndTime == 0)
    {
        return 0xFFFFFFFF;
    }

    // (792)
    Blizzard::Time::Timestamp uNow = GetNetworkTimestamp();
    if (suSeasonEndTime <= uNow)
    {
        return 0;
    }

    // (798)
    return (uint32)((suSeasonEndTime - uNow) / 1000000000);
}


void IsSeasonInProgress(int32 nUserIndex, IsSeasonInProgressCallback pfnCallback)
{
    if (!sbSeasonsDataReceived)
    {
        QueueNetworkEventForMainThreadAlways(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, IsSeasonInProgressCallback>, int>::bind<&OnlineService::internal::callback_base<1, IsSeasonInProgressCallback>::execute>(pfnCallback, FALSE));
    }
    else
    {
        // Ask the server for the time first, so the season state is current.
        LobbyService::Get(nUserIndex)->GetServerTime(
            [pfnCallback](BOOL bSuccess, const bdTimeStamp & tTimestamp)
            {
                BOOL bInProgress = FALSE;
                if (bSuccess)
                {
                    LobbyServiceInternal::OnServerTime(bSuccess, tTimestamp);
                    bInProgress = OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE;
                }

                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, IsSeasonInProgressCallback>, int>::bind<&OnlineService::internal::callback_base<1, IsSeasonInProgressCallback>::execute>(pfnCallback, bInProgress));
            }, false);
    }
}
bool GetGBIDBlacklistData(GBID gbid, uint32 * flags)
{
    GBIDBlacklistMap::const_iterator it = tBlacklistedGBIDs.find(gbid);
    if (it == tBlacklistedGBIDs.end())
    {
        return false;
    }

    if (flags)
    {
        *flags = it->second;
    }
    return true;
}







bool GetSNOBlacklistData(const SNOName & sno, uint32 * flags)
{
    SNOBlacklistMap::const_iterator it = tBlacklistedSNOs.find(sno);
    if (it == tBlacklistedSNOs.end())
    {
        return false;
    }

    if (flags)
    {
        *flags = it->second;
    }
    return true;
}








void LobbyService::Create(uint32 idTitle)
{
    // One lobby connection per local user.

    for (int32 i = 0; i < 4; ++i)
    {
        sptLobby[i] = ::new LobbyServiceInternal(i, idTitle);
    }
}




LobbyService * LobbyService::Get(int32 nUserIdx)
{
    // (887-889)

    return sptLobby[nUserIdx];
}






void LobbyService::Idle()
{
    // (899-900)

    for (int32 i = 0; i < 4; ++i)
        sptLobby[i]->IdleInternal();

    // (903-908)





    BOOL bCommunityBuffActive = FALSE;
    if (stTimeOffset.Exists())
    {
        // (911-912)

        Blizzard::Time::Timestamp tNow = GetNetworkTimestamp();
        if (stCommunityBuffStartTime != 0 &&
            stCommunityBuffEndTime > stCommunityBuffStartTime)
        {
            // (917)
            bCommunityBuffActive = tNow >= stCommunityBuffStartTime &&
                                   tNow < stCommunityBuffEndTime;
        }

        // (922)
        sEvaluateSeasonState(false);
    }

    // (925)
    if (bCommunityBuffActive != sbCommunityBuffEnabled)
    {
        sbCommunityBuffEnabled = bCommunityBuffActive;
        // (929)
        if (sbCommunityBuffEnabled)
        {
            // (932)
            SPlayerSetPerkBuffs(sflXPBonus, sflLegendaryFindBonus, sflGoldFindBonus, sbDoubleTreasureGoblins,
                sbDoubleBloodShards, sbDoubleRiftKeystones, sbEasterEggWorldEnabled, sbDoubleBountyBags);
        }
        else
        {
            SPlayerSetPerkBuffs(0.0f, 0.0f, 0.0f, FALSE, FALSE, FALSE, FALSE, FALSE);
        }
    }
}

void LobbyService::ConnectAll()
{
    // (948-951)
    for (int32 i = 0; i < 4; ++i)
        sptLobby[i]->Connect();
}






void LobbyService::DisconnectAll()
{
    // (960-963)
    for (int32 i = 0; i < 4; ++i)
        sptLobby[i]->Disconnect();
}


















void LobbyService::AttemptConnectOnUser(int32 nUserIndex)
{
    // (984-985)
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        sptLobby[nUserIndex]->AttemptToConnectOnNX64();
    }
}

BOOL LobbyService::IsUserActive(int32 nUserIndex)
{
    // (993-994)
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        return sptLobby[nUserIndex]->IsActive();
    }

    return FALSE;
}

BOOL LobbyService::IsUserConnected(int32 nUserIndex)
{
    // (1004-1005)
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        return sptLobby[nUserIndex]->IsConnected();
    }

    return FALSE;
}

BOOL LobbyService::ShouldShowReconnectOption(int32 nUserIndex)
{
    if (GamerProfile::IsActive(nUserIndex) &&
        !GamerProfile::IsGuest(nUserIndex) &&
        GetNetworkMode() != NET_ADHOC)
    {
        if (!GamerProfile::IsSignedInOnline(nUserIndex) || !IsUserActive(nUserIndex))
        {
            return TRUE;
        }
    }

    return FALSE;
}

void LobbyService::VerifyNSOSubscriptionAsync(int32 nUserIndex, VerifyNSOCallback pfnCallback)
{
    // (1031-1032)

    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        sptLobby[nUserIndex]->VerifyNSOSubscriptionAsync_Internal(pfnCallback);
    }
    else
    {
        pfnCallback();
    }
}
void LobbyService::RunTokenAuthentication(int32 nUserIndex)
{
    // (1043-1044)
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        sptLobby[nUserIndex]->RunTokenAuthentication_Internal();
    }
}





BOOL LobbyService::IsAnyActive()
{
    // (1056-1059)
    for (int32 i = 0; i < 4; ++i)
    {
        if (sptLobby[i]->IsActive())
        {
            return TRUE;
        }
    }

    return FALSE;
}













LobbyServiceInternal::LobbyServiceInternal(int32 nUserIndex, const bdTitleID idTitle)
    : LobbyService()
    , bdLobbyEventHandler()
    , m_nUserIndex(nUserIndex)
    , m_idTitle(idTitle)
    , m_eConnectionState(CONNSTATE_INACTIVE)
    , m_bWasConnected(false)
    , m_ptLobbyService(NULL)
    , m_ptAuthService(NULL)
    , m_bRunningTokenAuthentication(FALSE)
    , m_bAttemptConnectForNX64(FALSE)
    , m_bReconnect(FALSE)
    , m_bReconnectWhenSignedIn(FALSE)
    , m_uLastReconnectAttemptMS(0)
    , m_uReconnectDelayMS(0)
    , m_idOnlineUser(0)
    , m_bCheckedForBnetAccount(FALSE)
    , m_nBalanceVersion(0)
    , m_nFileInProgress(FILE_NONE)


    , m_bFatalErrorOccurred(false)

{
    m_pfnVerifyNSOCallback.Clear();

}




LobbyServiceInternal::~LobbyServiceInternal()
{
    // (1113-1115)

    m_tTaskManager.Clear();

    ::delete m_ptLobbyService;
    ::delete m_ptAuthService;
}




void LobbyServiceInternal::IdleInternal()

{
    int32 nPrimaryUserIdx = GamerProfile::GetPrimaryProfileUserIndex(); // Inference: the primary-user flag below is the original index != -1 test.
    BOOL bIsThereAPrimaryUser = nPrimaryUserIdx != -1;

    if (IsConnected() || IsConnecting())
    {

        if (!bIsThereAPrimaryUser ||
            !GamerProfile::IsActive(m_nUserIndex) ||
            !GamerProfile::IsSignedInOnline(m_nUserIndex))
        {
            m_bReconnectWhenSignedIn = TRUE;
            Disconnect();
        }
    }
    else if (bIsThereAPrimaryUser &&
        GamerProfile::IsActive(m_nUserIndex) &&
        IsPhysicalNetworkConnected())
    {
        if (m_bReconnectWhenSignedIn)
        {
            // Inference: this connection call shares the line-zero virtual-call tail.
            Connect();
        }




        else if (!IsActive())
        {
            if (m_eConnectionState != CONNSTATE_INACTIVE

                || m_bAttemptConnectForNX64)
            {
                // Inference: this connection call shares the line-zero virtual-call tail.
                Connect();
            }
        }
    }






    if (m_pfnVerifyNSOCallback)
    {
        int32 nPrimaryUserIdx = GamerProfile::GetPrimaryProfileUserIndex();
        // Inference: unresolved subscription starts false, confirmed by the original zero register.
        BOOL bSubscriptionHasBeenResolved = FALSE;
        if (m_ptAuthService)
        {

            if (!m_bRunningTokenAuthentication)
            {



                // Inference: zero is BD_NSO_SUBSCRIPTION_NOT_SET (bdAuthSwitch.h22).
                bSubscriptionHasBeenResolved = m_ptAuthService->getNSOSubscriptionStatus() != 0;
            }
        }




        if (nPrimaryUserIdx == -1 ||
            sbNetworkThreadDone ||
            !GamerProfile::IsActive(m_nUserIndex) ||
            !IsPhysicalNetworkConnected() ||
            bSubscriptionHasBeenResolved ||
            (!GamerProfile::IsEnsuringAccountAvailability(m_nUserIndex) && !GamerProfile::IsSignedInOnline(m_nUserIndex)) ||
            GamerProfile::IsGuest(m_nUserIndex))
        {
            m_bRunningTokenAuthentication = FALSE;
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_0<void, OnlineService::internal::callback_base<0, VerifyNSOCallback> >::bind<&OnlineService::internal::callback_base<0, VerifyNSOCallback>::execute>(m_pfnVerifyNSOCallback));
            m_pfnVerifyNSOCallback.Clear();
        }
    }


    if (IsConnected() &&
        !m_bCheckedForBnetAccount)
    {
        m_bCheckedForBnetAccount = TRUE;
    }

    if (IsConnected() &&
        m_nUserIndex == nPrimaryUserIdx)
    {

        Blizzard::Time::Timestamp now = Blizzard::Time::GetTimestamp();
        int64 uTimeDelta = now - suLastTimeCheck;
        // Inference: original unsigned comparison is a 60000000000-nanosecond interval.
        if (uTimeDelta > 60000000000ULL)
        {

            GetServerTime(OnServerTime, true);
        }

        suLastTimeCheck = now;
    }

    if (m_ptLobbyService)
    {
        m_ptLobbyService->pump();
    }

    switch (m_eConnectionState)
    {





























    case CONNSTATE_AUTH_START:
    {
        BOOL bAuthorizing;
        {
            GamerProfile::ScopedUsersReadLock tScopedLock;
            nn::account::UserHandle tUserHandle = GamerProfile::GetUserHandle(m_nUserIndex);

            nn::account::Nickname tUserNickName = GamerProfile::GetUserNickName(m_nUserIndex);

            ::delete m_ptAuthService;
            m_ptAuthService = ::new bdAuthSwitch(m_idTitle, stAuthEnvironmentStrings, BD_ENVIRONMENT_DEV, 443, BD_DEFAULT_ISS_ENV_JSON);

            SigmaMemoryCopy(m_szNickNameUsedForAuth, tUserNickName.name, sizeof(m_szNickNameUsedForAuth));

            bAuthorizing = m_ptAuthService->authorizeSwitch(tUserHandle, m_szNickNameUsedForAuth, 0);
        }


        if (!bAuthorizing)
        {
            Disconnect();
            SetReconnectDelay();
        }
        else
        {


            m_eConnectionState = CONNSTATE_AUTH_PENDING;
        }
        break;
    }
    case CONNSTATE_AUTH_PENDING:
    {
        bdAuth::bdStatus eStatus = m_ptAuthService->getStatus();
        if (eStatus == bdAuth::BD_READY)
        {



            // Inference: original NSO status values0/3 mean unresolved/no subscription (header22/25).
            bdUInt16 nsoSubscriptionRequest = m_ptAuthService->getNSOSubscriptionStatus();
            if (nsoSubscriptionRequest == 0 || nsoSubscriptionRequest == 3)
            {
                GamerProfile::UpdateHasNSOSubscription(m_nUserIndex, FALSE);
            }
            else
            {
                GamerProfile::UpdateHasNSOSubscription(m_nUserIndex, TRUE);
            }




            if (m_pfnVerifyNSOCallback)
            {
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_0<void, OnlineService::internal::callback_base<0, VerifyNSOCallback> >::bind<&OnlineService::internal::callback_base<0, VerifyNSOCallback>::execute>(m_pfnVerifyNSOCallback));
                m_pfnVerifyNSOCallback.Clear();
            }

            m_bRunningTokenAuthentication = FALSE;




            bdLobbyErrorCode eAuthError = m_ptAuthService->getErrorCode();
            if (eAuthError != BD_AUTH_NO_ERROR && eAuthError != BD_AUTH_CREATE_USERNAME_EXISTS)
            {

                Disconnect();

                SetCustomReconnectDelay(s_uNX64TokenFailDelay);
                break;
            }







            bdEnvironment eEnvironment;
            if (!m_ptAuthService->getEnvironment(eEnvironment))
            {
                Disconnect();
                SetReconnectDelay();
                break;
            }



            const char * szLobbyAddress = stLobbyEnvironmentStrings.get(eEnvironment);
            if (!szLobbyAddress)
            {
                Disconnect();
                SetReconnectDelay();
                break;
            }



            ::delete m_ptLobbyService;
            m_ptLobbyService = ::new bdLobbyService(szLobbyAddress, 3074);
            m_ptLobbyService->registerEventHandler(this);

            m_eConnectionState = CONNSTATE_RESOLVE_PENDING;
        }
        break;
    }
    case CONNSTATE_RESOLVE_PENDING:
    {
        bdGetHostByName::bdStatus eLobbyDNSStatus;

        if (m_ptLobbyService->pumpDNSLookup(eLobbyDNSStatus))
        {

            m_eConnectionState = CONNSTATE_CONNECT;
        }
        else if (eLobbyDNSStatus != bdGetHostByName::BD_LOOKUP_PENDING)
        {
            Disconnect();
            SetReconnectDelay();
        }
        break;
    }





    case CONNSTATE_CONNECT:
    {
        m_idOnlineUser = m_ptAuthService->getAuthTicket()->m_userID;
        BOOL bConnecting = m_ptLobbyService->connect(m_ptAuthService->getAuthInfo(), true);

        if (bConnecting)
        {
            m_eConnectionState = CONNSTATE_CONNECTING;
            m_bReconnect = FALSE;
        }
        else
        {

            Disconnect();
            SetReconnectDelay();
        }
        break;
    }
    case CONNSTATE_CONNECTING:
    {
        bdLobbyConnection::Status eStatus = m_ptLobbyService->getStatus();
        switch (eStatus)
        {
        case bdLobbyConnection::BD_NOT_CONNECTED:
        case bdLobbyConnection::BD_DISCONNECTING:
        case bdLobbyConnection::BD_DISCONNECTED:

            m_eConnectionState = CONNSTATE_DISCONNECTING;
            SetReconnectDelay();
            break;









        case bdLobbyConnection::BD_CONNECTED:
        {
            m_eConnectionState = CONNSTATE_CONNECTED;
            m_uReconnectDelayMS = s_uInitialReconnectDelayMS;

            ++ms_nNumLobbyServicesConnected;
            m_bWasConnected = true;
            if (ms_nNumLobbyServicesConnected == 1)
            {
                suLastTimeCheck = Blizzard::Time::GetTimestamp();
                GetServerTime(
                    OnServerTime, true);


                StorageGetPublisherFile(
                    "Config.txt",
                    StorageGetFileCallback(OnlineService::internal::method_bind_0_2<void, LobbyServiceInternal, LobbyServiceInternal *, StorageResult, blz::shared_ptr<blz::string>, &LobbyServiceInternal::OnConfigFileRetrieved>(this)), true);

                sbHoldTasksForConfig = TRUE;

                const char * szSeasonsConfig = "Seasons.txt";







                sbSeasonsDownloadPending = true;
                StorageGetPublisherFile(
                    szSeasonsConfig,
                    StorageGetFileCallback(OnlineService::internal::method_bind_0_2<void, LobbyServiceInternal, LobbyServiceInternal *, StorageResult, blz::shared_ptr<blz::string>, &LobbyServiceInternal::OnSeasonsFileRetrieved>(this)), false);

                const char * szBlacklistConfig = "Blacklist.txt";







                sbBlacklistDownloadPending = true;
                StorageGetPublisherFile(
                    szBlacklistConfig,
                    StorageGetFileCallback(OnlineService::internal::method_bind_0_2<void, LobbyServiceInternal, LobbyServiceInternal *, StorageResult, blz::shared_ptr<blz::string>, &LobbyServiceInternal::OnBlacklistFileRetrieved>(this)), false);
            }


            m_bCheckedForBnetAccount = FALSE;

            m_tConnectEvent.NotifySubscribers(m_nUserIndex);
            break;
        }
        // Inference: unhandled state values follow the original no-op switch exit.
        default: break;
        }
        break;
    }
    case CONNSTATE_CONNECTED:
    {
        bdLobbyConnection::Status eStatus = m_ptLobbyService->getStatus();
        switch (eStatus)
        {
        case bdLobbyConnection::BD_NOT_CONNECTED:
        case bdLobbyConnection::BD_CONNECTING:
        case bdLobbyConnection::BD_DISCONNECTING:
        case bdLobbyConnection::BD_DISCONNECTED:

            m_eConnectionState = CONNSTATE_DISCONNECTING;
            SetReconnectDelay();
            OnDisconnect();
            break;
        // Inference: unhandled state values follow the original no-op switch exit.
        default: break;
        }









        if (m_bRunningTokenAuthentication && m_ptAuthService)
        {
            bdAuth::bdStatus eStatus = m_ptAuthService->getStatus();
            if (eStatus == bdAuth::BD_READY)
            {
                m_bRunningTokenAuthentication = FALSE;

                bdUInt16 nsoSubscriptionRequest = m_ptAuthService->getNSOSubscriptionStatus();
                // Inference: status0/3 are original BD_NSO_SUBSCRIPTION_NOT_SET/NO_SUBSCRIPTION.
                if (nsoSubscriptionRequest == 0 || nsoSubscriptionRequest == 3)
                {

                    GamerProfile::UpdateHasNSOSubscription(m_nUserIndex, FALSE);
                }
                else
                {
                    GamerProfile::UpdateHasNSOSubscription(m_nUserIndex, TRUE);
                }


                if (m_pfnVerifyNSOCallback)
                {
                    QueueNetworkEventForMainThread(OnlineService::internal::method_binder_0<void, OnlineService::internal::callback_base<0, VerifyNSOCallback> >::bind<&OnlineService::internal::callback_base<0, VerifyNSOCallback>::execute>(m_pfnVerifyNSOCallback));
                    m_pfnVerifyNSOCallback.Clear();
                }
            }
        }
        break;
    }



    case CONNSTATE_DISCONNECTING:
    {
        bdLobbyConnection::Status eStatus = m_ptLobbyService->getStatus();
        switch (eStatus)
        {
        case bdLobbyConnection::BD_NOT_CONNECTED:
        case bdLobbyConnection::BD_DISCONNECTED:
        {
            if (m_bReconnect

                && !m_bFatalErrorOccurred)
            {


                Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
                Blizzard::Time::Millisecond uTimeSinceLastReconnect = Blizzard::Time::GetTimeElapsed(m_uLastReconnectAttemptMS, now);
                if (uTimeSinceLastReconnect >= m_uReconnectDelayMS)
                {


                    if (m_ptLobbyService->getErrorCode() == BD_INVALID_USER_ID)
                    {
                        m_eConnectionState = CONNSTATE_INACTIVE;
                    }
                    else
                    {
                        m_eConnectionState = CONNSTATE_CONNECT;
                    }


                    m_uLastReconnectAttemptMS = now;
                }
            }
            else
            {

                m_eConnectionState = CONNSTATE_INACTIVE;
                m_bReconnect = FALSE;
            }
            break;
        }
        case bdLobbyConnection::BD_CONNECTING:
        case bdLobbyConnection::BD_CONNECTED:
        { // Inference: original fatal assertion reports line1610 with stripped text.
            DisplayInternalError(NULL, "", 1610, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);

            break;
        }
        // Inference: unhandled state values follow the original no-op switch exit.
        default: break;
        }
        break;
    }





    case CONNSTATE_DISCONNECTED:
    {
        bdLobbyConnection::Status eStatus = m_ptLobbyService->getStatus();
        if (eStatus != bdLobbyConnection::BD_NOT_CONNECTED && eStatus != bdLobbyConnection::BD_DISCONNECTED)
        {

            // Inference: original fatal assertion reports this source line, with stripped text.
            DisplayInternalError(NULL, "", 1631, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    }
    // Inference: unhandled state values follow the original no-op switch exit.
    default: break;
    }



    TaskManager::ProcessMode eProcessMode;

    if (!IsConnected())
    {
        if (IsConnecting())
        {
            eProcessMode = TaskManager::PROCESS_HOLD_NOT_STARTED_TASKS;
        } else {
            eProcessMode = TaskManager::PROCESS_FAIL_NOT_STARTED_TASKS;
        }
    }


    else if (sbHoldTasksForConfig)
    {
        eProcessMode = TaskManager::PROCESS_HOLD_NOT_STARTED_TASKS;
    } else {
        eProcessMode = TaskManager::PROCESS_CAN_START_TASKS;
    }
    m_tTaskManager.Process(eProcessMode);
}



// 1696-1705 inferred: each branch sets the time then the state (SimplifyCFG sinks both into one tail: state kept in x20 across the call, merged stores have no row, call row 1703:37 is 8-space indented).
void LobbyServiceInternal::Connect()
{
    // Only a signed-in, non-guest profile gets a lobby connection, and only while the network
    // thread runs.





    int32 nPrimaryUserIdx = GamerProfile::GetPrimaryProfileUserIndex();
    if (nPrimaryUserIdx == -1 ||
        sbNetworkThreadDone ||
        !GamerProfile::IsActive(m_nUserIndex) ||
        !GamerProfile::IsSignedInOnline(m_nUserIndex) ||
        GamerProfile::IsGuest(m_nUserIndex))
    {
        // failure form inferred from the original columns (1666/1667 start with '!', 1668 does not); these stores are tail-merged with the NET_ADHOC ones
        m_eConnectionState = CONNSTATE_INACTIVE; m_bReconnect = FALSE;
        return;
    }



    m_bFatalErrorOccurred = false;

    m_bRunningTokenAuthentication = FALSE;
    m_bAttemptConnectForNX64 = FALSE;

    if (seNetworkMode == NET_ADHOC)
    {
        m_eConnectionState = CONNSTATE_INACTIVE; m_bReconnect = FALSE;
        return;
    }



    m_bReconnectWhenSignedIn = FALSE;
    if (!m_bReconnect)
    {
        m_uReconnectDelayMS = s_uInitialReconnectDelayMS;
    }

    if (m_eConnectionState == CONNSTATE_INACTIVE)
    {
        m_uLastReconnectAttemptMS = Blizzard::Time::Milliseconds();
        m_eConnectionState = CONNSTATE_AUTH_START;
    }
    else if (m_eConnectionState == CONNSTATE_DISCONNECTED)
    {
        m_uLastReconnectAttemptMS = Blizzard::Time::Milliseconds();
        m_eConnectionState = CONNSTATE_CONNECT;
    }
}




void LobbyServiceInternal::Disconnect()
{
    // A pending NSO check is answered now.
    if (m_pfnVerifyNSOCallback)
    {
        QueueNetworkEventForMainThread(OnlineService::internal::method_binder_0<void, OnlineService::internal::callback_base<0, VerifyNSOCallback> >::bind<&OnlineService::internal::callback_base<0, VerifyNSOCallback>::execute>(m_pfnVerifyNSOCallback));
        m_pfnVerifyNSOCallback.Clear();
    }
    m_bRunningTokenAuthentication = FALSE;

    // (1721-1722)
    switch (m_eConnectionState)
    {
    case CONNSTATE_RESOLVE:
    case CONNSTATE_RESOLVE_PENDING:
    case CONNSTATE_AUTH_START:
    case CONNSTATE_AUTH_PENDING:
    case CONNSTATE_CONNECT:
        // Nothing is connected yet.




        m_eConnectionState = CONNSTATE_INACTIVE;
        break;

    case CONNSTATE_CONNECTING:
    case CONNSTATE_CONNECTED:
        // (1740-1742)


        m_ptLobbyService->disconnect();
        m_eConnectionState = CONNSTATE_DISCONNECTING;
        OnDisconnect();
        break;

    case CONNSTATE_DISCONNECTING:
        // (1749-1751)


        m_bReconnect = FALSE;
        break;

    default:
        break;
    }
}










class StorageGetPublisherFileTask : public LobbyTask
{
public:

    StorageGetPublisherFileTask(LobbyService * ptLobbyService, const char * sFilename, StorageGetFileCallback pfnCallback)
        : LobbyTask(ptLobbyService)
        // (1774-1776)
        , m_szFilename(sFilename)
        , m_pszBuffer(blz::make_shared<blz::string>())
        , m_pfnCallback(pfnCallback)
    {
        // (1781)
        m_tResult.m_fileData = m_pszBuffer->modify_data(16 * 1024);
        m_tResult.m_fileSize = m_pszBuffer->size();
    }


    virtual void Start()
    {
        // (1787)
        bdStorage * storage = m_ptLobbyService->GetStorage();
        m_ptRemoteTask = storage->getPublisherFile(m_szFilename.c_str(), &m_tResult);
    }


    virtual void OnComplete()
    {
        // (1795-1797)
        m_pszBuffer->resize(m_tResult.m_fileSize);
        m_pfnCallback(STORAGE_SUCCESS, m_pszBuffer);
    }



    virtual void OnFailed()
    {
        // (1804)
        if (!m_ptLobbyService->IsConnected())
        {
            m_pfnCallback(STORAGE_FAILED_NOT_CONNECTED, blz::shared_ptr<blz::string>());
        }
        else if (m_ptRemoteTask->getErrorCode() == BD_NO_FILE)
        {
            m_pfnCallback(STORAGE_FAILED_FILE_NOT_FOUND, blz::shared_ptr<blz::string>());
        }
        else
        {
            m_pfnCallback(STORAGE_FAILED, blz::shared_ptr<blz::string>());
        }
    }





    blz::embedded_string<128> m_szFilename;


    blz::shared_ptr<blz::string> m_pszBuffer;
    bdFileData m_tResult;


    StorageGetFileCallback m_pfnCallback;
};



void LobbyServiceInternal::StorageGetPublisherFile(const char * szFilename, StorageGetFileCallback pfnCallback, bool bStartImmediately)
{
    // (1837-1838)
    StorageGetPublisherFileTask * ptTask = ::new StorageGetPublisherFileTask(this, szFilename, pfnCallback);
    if (bStartImmediately)
    {
        ptTask->Start();
    }
}










class ListFilesByOwnerTask : public LobbyTask
{
public:

    ListFilesByOwnerTask(int32 nUserIndex, LobbyService * ptLobbyService, bdUserAccountID idUser, const char * szContext, const char * szFilename, StorageFindUserFileCallback pfnCallback)
        : LobbyTask(ptLobbyService)
        , m_nUserIndex(nUserIndex)
        , m_idUser(idUser)
        , m_szContext(szContext ? szContext : "")
        , m_szFilename(szFilename)
        , m_pfnCallback(pfnCallback)
    {
    }







    virtual void Start()
    {
        // (1876)
        bdUserID idUser;
        bdStorage * ptStorage = m_ptLobbyService->GetStorage(m_szContext.c_str(), idUser);

        // A file owned by someone else.
        if (m_idUser != bdUserAccountID())
        {
            idUser = m_idUser;
        }

        m_ptRemoteTask = ptStorage->listFilesByOwner(idUser, 0, &m_tFileInfo, 1, 0, m_szFilename.c_str());
    }

    virtual void OnComplete()
    {
        // (1891)
        uint32 uNumResults = m_ptRemoteTask->getNumResults();
        if (uNumResults == 0 || m_tFileInfo.m_fileSize == 0)
        {
            m_pfnCallback(STORAGE_FAILED_FILE_NOT_FOUND, m_tFileInfo);
        }
        else
        {
            m_pfnCallback(STORAGE_SUCCESS, m_tFileInfo);
        }
    }




    virtual void OnFailed()
    {
        // (1908)
        if (!m_ptLobbyService->IsConnected())
        {
            m_pfnCallback(STORAGE_FAILED_NOT_CONNECTED, m_tFileInfo);
        }
        else
        {
            m_pfnCallback(STORAGE_FAILED, m_tFileInfo);
        }
    }



    int32 m_nUserIndex;
    bdUserAccountID m_idUser;
    blz::embedded_string<128> m_szContext;
    blz::embedded_string<128> m_szFilename;
    StorageFindUserFileCallback m_pfnCallback;

    bdUserStorageFileInfo m_tFileInfo;
};



void LobbyServiceInternal::StorageFindUserFile(bdUserAccountID idUser, const char * szContext, const char * szFilename, StorageFindUserFileCallback pfnCallback)
{
    // (1934-1935)
    ::new ListFilesByOwnerTask(m_nUserIndex, this, idUser, szContext, szFilename, pfnCallback);
}











class StorageGetUserFileTask : public LobbyTask
{
public:

    StorageGetUserFileTask(int32 nUserIndex, LobbyService * ptLobbyService, const char * szContext, const bdUserStorageFileInfo & tFileInfo, StorageGetFileCallback pfnCallback)
        : LobbyTask(ptLobbyService)
        , m_nUserIndex(nUserIndex)
        , m_pfnCallback(pfnCallback)
        , m_szContext(szContext ? szContext : "")
        , m_tFileInfo(tFileInfo)
        , m_pszBuffer(blz::make_shared<blz::string>())
    {
        // (1961-1963)
        m_tFileData.m_fileData = m_pszBuffer->modify_data(m_tFileInfo.m_fileSize);
        m_tFileData.m_fileSize = m_pszBuffer->size();
    }





    virtual ~StorageGetUserFileTask()
    {
    }

    virtual void Start()
    {
        // (1975)
        bdUserID idUser;
        bdStorage * storage = m_ptLobbyService->GetStorage(m_szContext.c_str(), idUser);

        m_ptRemoteTask = storage->getFile(m_tFileInfo.m_fileName, &m_tFileData, m_tFileInfo.m_ownerID);
    }


    virtual void OnComplete()
    {
        // (1985)
        m_pfnCallback(STORAGE_SUCCESS, m_pszBuffer);
    }


    virtual void OnFailed()
    {
        // (1992)
        if (!m_ptLobbyService->IsConnected())
        {
            m_pfnCallback(STORAGE_FAILED_NOT_CONNECTED, blz::shared_ptr<blz::string>());
        }
        else
        {
            m_pfnCallback(STORAGE_FAILED, blz::shared_ptr<blz::string>());
        }
    }



    int32 m_nUserIndex;
    StorageGetFileCallback m_pfnCallback;


    blz::embedded_string<128> m_szContext;
    bdUserStorageFileInfo m_tFileInfo;
    blz::shared_ptr<blz::string> m_pszBuffer;
    bdFileData m_tFileData;
};



void LobbyServiceInternal::StorageGetUserFile(const char * szContext, const bdUserStorageFileInfo & tFileInfo, StorageGetFileCallback pfnCallback)
{
    // (2019-2020)
    ::new StorageGetUserFileTask(m_nUserIndex, this, szContext, tFileInfo, pfnCallback);
}










class StoragePublishUserFileTask : public LobbyTask
{
public:

    StoragePublishUserFileTask(int32 nUserIndex, LobbyService * ptLobbyService, const char * szContext, const char * szFilename, blz::string szData, StoragePublishUserFileCallback pfnCallback)
        // (2037-2042)






        : LobbyTask(ptLobbyService)
        , m_nUserIndex(nUserIndex)
        , m_pfnCallback(pfnCallback)
        , m_szContext(szContext ? szContext : "")
        , m_szFilename(szFilename)
        , m_szData(blz::move(szData))
    {
    }
    virtual ~StoragePublishUserFileTask()
    {
    }

    virtual void Start()
    {
        // (2058)
        bdUserID idUser;
        bdStorage * storage = m_ptLobbyService->GetStorage(m_szContext.c_str(), idUser);

        m_ptRemoteTask = storage->uploadFile(m_szFilename.c_str(), m_szData.c_str(), m_szData.size(), bdFileInfo::BD_VISIBLE_PRIVATE, idUser, NULL);
    }


    virtual void OnComplete()
    {
        m_pfnCallback(STORAGE_SUCCESS);
    }



    virtual void OnFailed()
    {
        // (2075)
        if (!m_ptLobbyService->IsConnected())
        {
            m_pfnCallback(STORAGE_FAILED_NOT_CONNECTED);
        }
        else
        {
            m_pfnCallback(STORAGE_FAILED);
        }
    }



    int32 m_nUserIndex;
    StoragePublishUserFileCallback m_pfnCallback;


    blz::embedded_string<128> m_szContext;
    blz::embedded_string<128> m_szFilename;
    blz::string m_szData;
};


class SendGlobalInstantMessageTask : public LobbyTask
{
public:

    SendGlobalInstantMessageTask(LobbyService * ptLobbyService, OnlineUserId idRecipient, const void * const ptMessage, const bdUInt uMsgSize)
        // (2103-2107)




        : LobbyTask(ptLobbyService)
        , m_idRecipient(idRecipient)
        , m_uMsgSize(uMsgSize)
    {
        // (2112-2114)


        SigmaMemoryCopy(m_szMessageBuffer, ptMessage, uMsgSize);
    }

    virtual void Start()
    {
        // (2119)
        m_ptRemoteTask = m_ptLobbyService->GetMessaging()->sendGlobalInstantMessage(m_idRecipient, m_szMessageBuffer, m_uMsgSize);
    }


    virtual void OnComplete()
    {
        // (2126-2128)


    }
    virtual void OnFailed()
    {
        // (2133-2141)








    }

    OnlineUserId m_idRecipient;
    char m_szMessageBuffer[4096];
    bdUInt m_uMsgSize;
};





void LobbyServiceInternal::StoragePublishUserFile(const char * szContext, const char * szFilename, blz::string szData, StoragePublishUserFileCallback pfnCallback)
{
    // (2155-2159)



    ::new StoragePublishUserFileTask(m_nUserIndex, this, szContext, szFilename, blz::move(szData), pfnCallback);
}








class LookupUserAccountsTask : public LobbyTask
{
public:
    LookupUserAccountsTask(LobbyService * ptLobbyService, OnlineUserId * pidUsers, uint32 uNumUsers, LobbyService::LookupUserAccountsCallback pfnCallback)
        // (2173-2176)



        : LobbyTask(ptLobbyService)
        , m_pfnCallback(pfnCallback)
        , m_uNumUsers(uNumUsers)
    {
        for (uint32 i = 0; i < m_uNumUsers; ++i)
        {
            m_aidUsers[i] = pidUsers[i];
        }
    }

    virtual void Start()
    {
        // (2189-2190)
        bdOnlineUserID atUserIds[50];

        for (uint32 i = 0; i < m_uNumUsers; ++i)
        {
            atUserIds[i] = m_aidUsers[i];
        }

        m_ptRemoteTask = m_ptLobbyService->GetLobbyService()->getTitleUtilities()->getUserNames(atUserIds, m_uNumUsers, m_atUserInfo);
    }


    virtual void OnComplete()
    {
        if (m_pfnCallback)
        {
            // (2205-2206)
            bdOnlineUserID aBDUserIds[50];
            CompileResults(aBDUserIds);


            m_pfnCallback(TRUE, m_aidUsers, aBDUserIds, m_uNumUsers);
        }
    }
    virtual void OnFailed()
    {
        if (m_pfnCallback)
        {
            // (2217)
            bdOnlineUserID aBDUserIds[50];
            for (uint32 i = 0; i < 50; ++i)
            {
                aBDUserIds[i] = 0;
            }

            m_pfnCallback(FALSE, m_aidUsers, aBDUserIds, m_uNumUsers);
        }
    }

    LobbyService::LookupUserAccountsCallback m_pfnCallback;

    OnlineUserId m_aidUsers[50];
    bdUserInfo m_atUserInfo[50];
    uint32 m_uNumUsers;


    void CompileResults(bdOnlineUserID * aptUserInfo)
    {
        // (2237)
        uint32 uNumResults = m_ptRemoteTask->getNumResults();

        for (uint32 nUserIndex = 0; nUserIndex < m_uNumUsers; ++nUserIndex)
        {
            OnlineUserId idUser = m_aidUsers[nUserIndex];

            aptUserInfo[nUserIndex] = 0;

            for (uint32 nResultIndex = 0; nResultIndex < uNumResults; ++nResultIndex)
            {
                // (2248-2249)


                if (idUser == m_atUserInfo[nResultIndex].m_userID)
                {
                    aptUserInfo[nUserIndex] = idUser;
                    break;
                }
            }
        }
    }
};


void LobbyServiceInternal::LookupUserAccounts(OnlineUserId * pidUsers, uint32 uNumUsers, LookupUserAccountsCallback pfnCallback)
{
    // (2264-2265)
    ::new LookupUserAccountsTask(this, pidUsers, uNumUsers, pfnCallback);
}










// 2.7.7: GetLinkedAccountsTask is gone (the linked-accounts feature was removed).
























































class IncrementCountersTask : public LobbyTask
{
public:
    IncrementCountersTask(LobbyService * ptLobbyService, blz::vector<bdCounterValue> & atCounters)
        // (2343-2344)

        : LobbyTask(ptLobbyService)
        , m_atCounters(blz::move(atCounters))
    {
    }

    virtual void Start()
    {
        // (2352)
        m_ptRemoteTask = m_ptLobbyService->GetLobbyService()->getCounter()->incrementCounters(m_atCounters.data(), m_atCounters.size());
    }


    blz::vector<bdCounterValue> m_atCounters;
};



// NON_MATCHING: 824 vs 616 B; ours inlines blz::vector<bdCounterValue>::_assign_rv (the original calls it; standalone it is byte-identical). Inliner: original defers _uninitialized_copy in _construct (needs its cost >= ~151, ours 130)
void LobbyServiceInternal::UpdateGlobalCounters(
    const CounterVector & atCounters)
{
    blz::vector<bdCounterValue> atBDCounters;

    for (const blz::pair<GlobalCounter, int32> & tPair : atCounters)
    {
        bdCounterValue & tBDCounter = atBDCounters.push_back();
        tBDCounter.m_counterID = tPair.first;
        tBDCounter.m_counterValue = tPair.second;
    }

    ::new IncrementCountersTask(this, atBDCounters);
}








class GetServerTimeTask : public LobbyTask
{
public:

    GetServerTimeTask(LobbyService * ptLobbyService, LobbyService::GetServerTimeCallback pfnCallback)
        : LobbyTask(ptLobbyService)
        , m_pfnCallback(pfnCallback)
    {
        // (2392-2393)

    }

    virtual void Start()
    {
        // (2398-2399)
        m_ptRemoteTask = m_ptLobbyService->GetLobbyService()->getTitleUtilities()->getServerTime(&m_tTimestamp);
    }


    virtual void OnComplete()
    {
        // (2405-2406)
        sbTimeCheckInProgress = false;
        m_pfnCallback(TRUE, m_tTimestamp);
    }

    virtual void OnFailed()
    {
        // (2412-2413)
        sbTimeCheckInProgress = false;
        m_pfnCallback(FALSE, m_tTimestamp);
    }

    LobbyService::GetServerTimeCallback m_pfnCallback;

    bdTimeStamp m_tTimestamp;
};



void LobbyServiceInternal::GetServerTime(GetServerTimeCallback pfnCallback, bool bStartImmediately)
{
    // (2426-2428)
    sbTimeCheckInProgress = true;

    GetServerTimeTask * ptTask = ::new GetServerTimeTask(this, pfnCallback);
    if (bStartImmediately)
    {
        ptTask->Start();
    }
}






BOOL LobbyServiceInternal::SendGlobalInstantMessage(OnlineUserId idRecipient, const void * const ptMessage, const bdUInt uMsgSize)
{
    // (2443-2446)


    BOOL bConnected = IsConnected();
    if (bConnected && ptMessage != NULL && uMsgSize <= sizeof(SendGlobalInstantMessageTask::m_szMessageBuffer))
    {
        // (2452-2453)


        ::new SendGlobalInstantMessageTask(this, idRecipient, ptMessage, uMsgSize);
        return TRUE;
    }

    return FALSE;
}




void LobbyServiceInternal::RunTokenAuthentication_Internal()
{




    ScopedNetworkThreadLock tLock;

    GamerProfile::ScopedUsersReadLock tScopedLock;
    nn::account::UserHandle tUserHandle = GamerProfile::GetUserHandle(m_nUserIndex);

    nn::account::Nickname tUserNickName = GamerProfile::GetUserNickName(m_nUserIndex);

    ::delete m_ptAuthService;
    m_ptAuthService = ::new bdAuthSwitch(m_idTitle, stAuthEnvironmentStrings, BD_ENVIRONMENT_DEV, 443, BD_DEFAULT_ISS_ENV_JSON);

    // Inference: original copies exactly the 32-byte stored authentication-name buffer.
    SigmaMemoryCopy(m_szNickNameUsedForAuth, tUserNickName.name, sizeof(m_szNickNameUsedForAuth));
    bdBool bAuthorizing = m_ptAuthService->authorizeSwitch(tUserHandle, m_szNickNameUsedForAuth, 0);


    if (bAuthorizing)
    {
        m_bRunningTokenAuthentication = TRUE;
    }
}




void LobbyServiceInternal::VerifyNSOSubscriptionAsync_Internal(VerifyNSOCallback pfnCallback)
{
    // (2495)
    ScopedNetworkThreadLock tLock;

    int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();

    // (2500)
    if (nPrimaryUserIndex == -1 ||
        !GamerProfile::IsActive(m_nUserIndex) ||
        !IsPhysicalNetworkConnected())
    {
        pfnCallback();
    }
    else
    {
        m_pfnVerifyNSOCallback = pfnCallback;
    }
}











class LeaderboardWriteStatsTask : public LobbyTask
{
public:

    LeaderboardWriteStatsTask(LobbyService * ptLobbyService, blz::vector<LeaderboardStatsInfo> & atStats, LeaderboardPostCallback pfnCallback)
        : LobbyTask(ptLobbyService)
        , m_atStats(blz::move(atStats))
        , m_pfnCallback(pfnCallback)
    {
        // (2532-2533)
    }



    virtual void Start()
    {
        // (2539-2541)


        bdStatsInfo * atStats[4];

        blz::transform(m_atStats.begin(), m_atStats.end(), atStats, [](LeaderboardStatsInfo & tStat) -> bdStatsInfo * { return &tStat; });

        m_ptRemoteTask = m_ptLobbyService->GetStats()->writeStats(atStats, m_atStats.size());
    }


    virtual void OnComplete()
    {
        // (2552)
        m_pfnCallback(LeaderboardResult::SUCCESS);
    }



    virtual void OnFailed()
    {
        // (2560)
        LeaderboardResult eResult = LeaderboardResult::FAILED;
        if (!m_ptLobbyService->IsConnected())
        {
            eResult = LeaderboardResult::FAILED_NOT_CONNECTED;
        }
        m_pfnCallback(eResult);
    }








    blz::vector<LeaderboardStatsInfo> m_atStats;
    LeaderboardPostCallback m_pfnCallback;
};



void LobbyServiceInternal::LeaderboardWriteStats(blz::vector<LeaderboardStatsInfo, blz::allocator<LeaderboardStatsInfo> > & atStats, LeaderboardPostCallback pfnCallback)
{
    // (2583-2585)
    for (LeaderboardStatsInfo & tStat : atStats)
    {
        tStat.m_entityID = m_idOnlineUser;
    }

    ::new LeaderboardWriteStatsTask(this, atStats, pfnCallback);
}










class LeaderboardReadStatsBaseTask : public LobbyTask
{
public:

    typedef LobbyService::LeaderboardReadStatsCallback Callback;

    LeaderboardReadStatsBaseTask(LobbyService * ptLobbyService, size_t uMaxResults, Callback pfnCallback)
        // (2609-2611)


        : LobbyTask(ptLobbyService)
        , m_pfnCallback(pfnCallback)
    {
        m_atStats.resize(uMaxResults);
    }

    LeaderboardReadStatsBaseTask(LobbyService * ptLobbyService, const LeaderboardKeyAndPartition & idLeaderboard, size_t uMaxResults, Callback pfnCallback)
        // (2619-2622)



        : LeaderboardReadStatsBaseTask(ptLobbyService, uMaxResults, pfnCallback)
    {
        for (LeaderboardStatsInfo & tStat : m_atStats)
        {
            tStat.m_lbKey = idLeaderboard;
        }
    }

    virtual void OnComplete()
    {
        // (2633-2634)
        uint32 uNumResults = m_ptRemoteTask->getNumResults();
        m_atStats.resize(uNumResults);

        m_pfnCallback(LeaderboardResult::SUCCESS, m_atStats);
    }




    virtual void OnFailed()
    {
        // (2645)
        LeaderboardResult eResult = LeaderboardResult::FAILED;
        if (!m_ptLobbyService->IsConnected())
        {
            eResult = LeaderboardResult::FAILED_NOT_CONNECTED;
        }
        else if (m_ptRemoteTask->getErrorCode() == BD_NO_STATS_FOR_USER)
        {
            // Nothing posted yet is not an error.
            eResult = LeaderboardResult::SUCCESS;
        }

        m_atStats.resize(0);
        m_pfnCallback(eResult, m_atStats);
    }






    Callback m_pfnCallback;

    blz::vector<LeaderboardStatsInfo> m_atStats;
    blz::vector<bdStatsInfo *> m_aptStatsInfo;
};





class LeaderboardReadStatsByLeaderboardIdsTask : public LeaderboardReadStatsBaseTask
{
public:

    LeaderboardReadStatsByLeaderboardIdsTask(LobbyService * ptLobbyService, const blz::vector<LeaderboardKeyAndPartition> & idLeaderboardKeys, bdOnlineUserID idUser, Callback pfnCallback)
        // (2681-2684)



        : LeaderboardReadStatsBaseTask(ptLobbyService, idLeaderboardKeys.size(), pfnCallback)
        , m_idUser(idUser)
    {
        m_idDWLeaderboards.reserve(idLeaderboardKeys.size());

        for (const LeaderboardKeyAndPartition & idLeaderboardKey : idLeaderboardKeys)
        {
            uint32 dwLBID = LeaderboardToDemonwareLeaderboardId(idLeaderboardKey.m_id, idLeaderboardKey.m_scope, idLeaderboardKey.m_partition, false);
            m_idLeaderboardMap[dwLBID] = idLeaderboardKey;
            m_idDWLeaderboards.push_back(dwLBID);
        }
    }

    virtual void Start()
    {
        // (2701-2703)


        m_atStatsEx.resize(m_atStats.size());
        m_aptStatsInfoEx.resize(m_atStatsEx.size());
        for (size_t i = 0; i < m_atStats.size(); ++i)
        {
            m_atStatsEx[i].m_statsInfo = &m_atStats[i];
            m_aptStatsInfoEx[i] = &m_atStatsEx[i];
        }

        // (2711-2712)
        m_ptRemoteTask = m_ptLobbyService->GetStats()->readStatsByLeaderboardIDsAndEntityIDs(
            &m_idUser, m_idDWLeaderboards.data(), m_aptStatsInfoEx.data(), m_idDWLeaderboards.size(), 1);
    }








    virtual void OnComplete()
    {
        uint32 uNumResults = m_ptRemoteTask->getNumResults();

        for (uint32 i = 0; i < uNumResults; ++i)
        {
            LeaderboardStatsInfo & tStat = m_atStats[i];
            blz::map<uint32, LeaderboardKeyAndPartition>::iterator it = m_idLeaderboardMap.find(tStat.m_leaderboardID);
            if (it != m_idLeaderboardMap.end())
            {
                tStat.m_lbKey = it->second;
            }
        }

        LeaderboardReadStatsBaseTask::OnComplete();
    }


    bdOnlineUserID m_idUser;

    blz::vector<uint32> m_idDWLeaderboards;
    blz::map<uint32, LeaderboardKeyAndPartition> m_idLeaderboardMap;

    blz::vector<bdStatsInfoEx> m_atStatsEx;
    blz::vector<bdStatsInfoEx *> m_aptStatsInfoEx;
};




class LeaderboardReadStatsByPivotTask : public LeaderboardReadStatsBaseTask
{
public:

    LeaderboardReadStatsByPivotTask(LobbyService * ptLobbyService, const LeaderboardKeyAndPartition & idLeaderboard, bdOnlineUserID idUser, uint32 uMaxResults, Callback pfnCallback)
        // (2758-2762)




        : LeaderboardReadStatsBaseTask(ptLobbyService, idLeaderboard, uMaxResults, pfnCallback)
        , m_idLeaderboard(idLeaderboard)
        , m_idDWLeaderboard(LeaderboardToDemonwareLeaderboardId(idLeaderboard.m_id, idLeaderboard.m_scope, idLeaderboard.m_partition, false))
        , m_idUser(idUser)
        , m_uMaxResults(uMaxResults)
    {
    }


    virtual void Start()
    {
        // (2773)
        m_aptStatsInfo.resize(m_atStats.size());
        // (2775-2779)




        blz::transform(m_atStats.begin(), m_atStats.end(), m_aptStatsInfo.begin(), [](LeaderboardStatsInfo & tStat) -> bdStatsInfo * { return &tStat; });
        m_ptRemoteTask = m_ptLobbyService->GetStats()->readStatsByPivot(m_idDWLeaderboard,
            m_idUser,
            m_aptStatsInfo.data(),
            m_uMaxResults,
            NULL, 0);
    }

    LeaderboardKeyAndPartition m_idLeaderboard;
    bdLeaderBoardID m_idDWLeaderboard;
    bdOnlineUserID m_idUser;
    uint32 m_uMaxResults;
};




class LeaderboardReadStatsByEntityIdsTask : public LeaderboardReadStatsBaseTask
{
public:

    LeaderboardReadStatsByEntityIdsTask(LobbyService * ptLobbyService, const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<unsigned long> & aidUsers, Callback pfnCallback)
        // (2803-2806)



        : LeaderboardReadStatsBaseTask(ptLobbyService, idLeaderboard, aidUsers.size(), pfnCallback)
        , m_idLeaderboard(idLeaderboard)
        , m_idDWLeaderboard(LeaderboardToDemonwareLeaderboardId(idLeaderboard.m_id, idLeaderboard.m_scope, idLeaderboard.m_partition, false))
        , m_aidUsers(aidUsers)
    {
    }

    LeaderboardReadStatsByEntityIdsTask(LobbyService * ptLobbyService, const LeaderboardKeyAndPartition & idLeaderboard, bool bMetadata, const blz::vector<unsigned long> & aidUsers, Callback pfnCallback)
        // (2815-2819)




        : LeaderboardReadStatsByEntityIdsTask(ptLobbyService, idLeaderboard, aidUsers, pfnCallback)
    {
        m_idDWLeaderboard = LeaderboardToDemonwareLeaderboardId(idLeaderboard.m_id, idLeaderboard.m_scope, idLeaderboard.m_partition, bMetadata);
    }

    virtual void Start()
    {
        // (2827-2832)
        m_aptStatsInfo.resize(m_atStats.size());
        blz::transform(m_atStats.begin(), m_atStats.end(), m_aptStatsInfo.begin(), [](LeaderboardStatsInfo & tStat) -> bdStatsInfo * { return &tStat; });




        m_ptRemoteTask = m_ptLobbyService->GetStats()->readStatsByEntityID(m_idDWLeaderboard,
            m_aidUsers.data(), m_aidUsers.size(),
            m_aptStatsInfo.data(),
            NULL, 0);
    }



    LeaderboardKeyAndPartition m_idLeaderboard;
    bdLeaderBoardID m_idDWLeaderboard;
    blz::vector<unsigned long> m_aidUsers;
};



class LeaderboardReadStatsByRankTask : public LeaderboardReadStatsBaseTask
{
public:

    LeaderboardReadStatsByRankTask(LobbyService * ptLobbyService, const LeaderboardKeyAndPartition & idLeaderboard, uint32 uFirstRank, uint32 uMaxResults, Callback pfnCallback)
        // (2854-2858)




        : LeaderboardReadStatsBaseTask(ptLobbyService, idLeaderboard, uMaxResults, pfnCallback)
        , m_idLeaderboard(idLeaderboard)
        , m_idDWLeaderboard(LeaderboardToDemonwareLeaderboardId(idLeaderboard.m_id, idLeaderboard.m_scope, idLeaderboard.m_partition, false))
        , m_uFirstRank(uFirstRank)
        , m_uMaxResults(uMaxResults)
    {
    }

    virtual void Start()
    {
        // (2868-2870)


        m_aptStatsInfo.resize(m_atStats.size());
        blz::transform(m_atStats.begin(), m_atStats.end(), m_aptStatsInfo.begin(), [](LeaderboardStatsInfo & tStat) -> bdStatsInfo * { return &tStat; });

        // (2874)
        m_ptRemoteTask = m_ptLobbyService->GetStats()->readStatsByRank(m_idDWLeaderboard,
            m_uFirstRank,
            m_aptStatsInfo.data(),
            m_uMaxResults,
            NULL, 0);
    }


    LeaderboardKeyAndPartition m_idLeaderboard;
    bdLeaderBoardID m_idDWLeaderboard;
    uint32 m_uFirstRank;
    uint32 m_uMaxResults;
};




void LobbyServiceInternal::LeaderboardReadStatsByLeaderboardIds(const blz::vector<LeaderboardKeyAndPartition, blz::allocator<LeaderboardKeyAndPartition> > & idLeaderboards, LeaderboardReadStatsCallback pfnCallback)
{
    // (2895-2896)
    ::new LeaderboardReadStatsByLeaderboardIdsTask(this, idLeaderboards, m_idOnlineUser, pfnCallback);
}


void LobbyServiceInternal::LeaderboardReadStatsByPivot(const LeaderboardKeyAndPartition & idLeaderboard, uint32 uMaxResults, LeaderboardReadStatsCallback pfnCallback)
{
    // (2902-2903)
    ::new LeaderboardReadStatsByPivotTask(this, idLeaderboard, m_idOnlineUser, uMaxResults, pfnCallback);
}



void LobbyServiceInternal::LeaderboardReadStatsByEntityId(const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<unsigned long, blz::allocator<unsigned long> > & aidUsers, bool bIncludeSelf, LeaderboardReadStatsCallback pfnCallback)
{
    // (2910-2912)


    if (bIncludeSelf)
    {
        // (2915-2916)

        blz::vector<unsigned long> aidUsersCopy(aidUsers); aidUsersCopy.push_back(m_idOnlineUser);
        ::new LeaderboardReadStatsByEntityIdsTask(this, idLeaderboard, aidUsersCopy, pfnCallback);
    }
    else
    {
        ::new LeaderboardReadStatsByEntityIdsTask(this, idLeaderboard, aidUsers, pfnCallback);
    }
}

void LobbyServiceInternal::LeaderboardReadStatsByRank(const LeaderboardKeyAndPartition & idLeaderboard, uint32 uFirstRank, uint32 uMaxResults, LeaderboardReadStatsCallback pfnCallback)
{
    // (2928-2929)
    ::new LeaderboardReadStatsByRankTask(this, idLeaderboard, uFirstRank, uMaxResults, pfnCallback);
}




void LobbyServiceInternal::LeaderboardReadMetadata(const LeaderboardKeyAndPartition & idLeaderboard, bdOnlineUserID idUser, LeaderboardReadStatsCallback pfnCallback)
{
    // (2937-2939)


    blz::vector<unsigned long> idUsers = { idUser };
    ::new LeaderboardReadStatsByEntityIdsTask(this, idLeaderboard, true, idUsers, pfnCallback);
}




void LobbyServiceInternal::OnConfigFileRetrieved(
    StorageResult eResult,
    blz::shared_ptr<blz::string> pszFileData)
{
    sbHoldTasksForConfig = FALSE;

    stCommunityBuffStartTime = 0;
    stCommunityBuffEndTime = 0;
    sflXPBonus = 0.0f;
    sflLegendaryFindBonus = 0.0f;
    sflGoldFindBonus = 0.0f;
    sbDoubleTreasureGoblins = FALSE;
    sbDoubleBloodShards = FALSE;
    sbDoubleRiftKeystones = FALSE;
    sbEasterEggWorldEnabled = FALSE;
    sbDoubleBountyBags = FALSE;
    sbSeasonalGlobalLeaderboardsEnabled = TRUE;

    if (eResult != STORAGE_SUCCESS)
    {
        return;
    }

    KeyValueMap mapKeyValues;
    Prefs::ParseFileDataToKeyValues(pszFileData->modify_data(), pszFileData->size(), mapKeyValues);

    CRefString sValue;
    if (mapKeyValues.Lookup("HeroPublishFrequencyMinutes", sValue))
    {
        int32 nFrequency = atoi(sValue);
        HeroPublishing::SetPublishIntervalInMinutes(nFrequency);
    }

    if (mapKeyValues.Lookup("EnableCrossPlatformSaveMigration", sValue))
    {
        int32 nValue = atoi(sValue);
        SaveMigration::SetCrossPlatformSaveMigrationEnabled(nValue);
    }



    if (mapKeyValues.Lookup("CommunityBuffStart", sValue))
    {
        stCommunityBuffStartTime = Blizzard::Time::FromHTTPDate(sValue);
    }
    if (mapKeyValues.Lookup("CommunityBuffEnd", sValue))
    {
        stCommunityBuffEndTime = Blizzard::Time::FromHTTPDate(sValue);
    }
    if (mapKeyValues.Lookup("CommunityBuffXP", sValue))
    {
        sflXPBonus = atof(sValue);
    }
    if (mapKeyValues.Lookup("CommunityBuffLegendaryFind", sValue))
    {
        sflLegendaryFindBonus = atof(sValue);
    }
    if (mapKeyValues.Lookup("CommunityBuffGoldFind", sValue))
    {
        sflGoldFindBonus = atof(sValue);
    }
    if (mapKeyValues.Lookup("CommunityBuffDoubleGoblins", sValue))
    {
        int32 nValue = atoi(sValue);
        sbDoubleTreasureGoblins = nValue;
    }
    if (mapKeyValues.Lookup("CommunityBuffDoubleBloodShards", sValue))
    {
        int32 nValue = atoi(sValue);
        sbDoubleBloodShards = nValue;
    }
    if (mapKeyValues.Lookup("CommunityBuffDoubleRiftKeystones", sValue))
    {
        int32 nValue = atoi(sValue);
        sbDoubleRiftKeystones = nValue;
    }
    if (mapKeyValues.Lookup("CommunityBuffEasterEggWorld", sValue))
    {
        int32 nValue = atoi(sValue);
        sbEasterEggWorldEnabled = nValue;
    }
    if (mapKeyValues.Lookup("CommunityBuffDoubleBountyBags", sValue))
    {
        int32 nValue = atoi(sValue);
        sbDoubleBountyBags = nValue;
    }

    if (mapKeyValues.Lookup("SeasonalGlobalLeaderboardsEnabled", sValue))
    {
        int32 nValue = atoi(sValue);
        sbSeasonalGlobalLeaderboardsEnabled = nValue;
    }


    if (mapKeyValues.Lookup("MailTimeoutDaysMail", sValue))
    {
        int32 nValueDays = atoi(sValue);
        MailService::SetTimeout(MAIL_TYPE_MESSAGE, nValueDays);
    }
    if (mapKeyValues.Lookup("MailTimeoutDaysAvengers", sValue))
    {
        int32 nValueDays = atoi(sValue);
        MailService::SetTimeout(MAIL_TYPE_EVENT, nValueDays);
    }


    if (mapKeyValues.Lookup("UpdateVersion", sValue))
    {
        m_nBalanceVersion = atoi(sValue);
    }

    if (m_nBalanceVersion > 0)
    {

        // Inference: zero denotes no installed locale in the original comparison.
        XLocale eCurrentUpdateLocale = 0;
        XLocale eCurrentLocale = UnicodeText::GetCurrentLocale();

        for (int32 i = 0; i < NUM_FILE_TYPES; ++i)
        {
            m_atUpdateFiles[i].Clear();
        }



        if (UpdateManager::GetCurrentUpdateVersion(eCurrentUpdateLocale) != m_nBalanceVersion ||
            (eCurrentUpdateLocale != 0 && eCurrentUpdateLocale != eCurrentLocale))
        {






            m_atUpdateFiles[FILE_UPDATES_GLOBAL].m_szFilename.format("update-%d.cpk", m_nBalanceVersion);



            m_atUpdateFiles[FILE_UPDATES_LOCALE].m_szFilename.format("update-%d-%s.cpk", m_nBalanceVersion, XLocaleToString(eCurrentLocale));



            StorageGetPublisherFile(
                m_atUpdateFiles[FILE_UPDATES_GLOBAL].m_szFilename.c_str(),
                StorageGetFileCallback(OnlineService::internal::method_bind_0_2<void, LobbyServiceInternal, LobbyServiceInternal *, StorageResult, blz::shared_ptr<blz::string>, &LobbyServiceInternal::OnUpdateInfoRetrieved>(this)), false);
        }
    }




    else
    {
        UpdateManager::RemoveUpdates();
    }
}




void LobbyServiceInternal::OnSeasonsFileRetrieved(
    StorageResult eResult,
    blz::shared_ptr<blz::string> pszFileData)
{
    // The seasons file: a "[Season N]" header per season, then Pre/Start/End lines with
    // HTTP dates in quotes. Lines starting with '#' are comments.










    sbSeasonsDownloadPending = false;

    if (eResult != STORAGE_SUCCESS)
    {
        // No file means no seasons.
        if (eResult == STORAGE_FAILED_FILE_NOT_FOUND)
        {
            sbSeasonsDataReceived = true;
        }
        return;
    }


    sbSeasonsDataReceived = true;


    // DWARF: uNumSeasonsFound is the constant 0 over the whole function (never incremented)
    uint32 uNumSeasonsFound = 0;
    SeasonInfo * ptSeasonInfo = NULL;

    blz::string & szFileData = *pszFileData;

    const char * szDelimiters = "\r\n";
    size_t szDelimitersLen = strlen(szDelimiters);
    size_t startPos = szFileData.find_first_not_of(szDelimiters, 0, szDelimitersLen);
    while (startPos != blz::string::npos)
    {
        const char * szLine = szFileData.c_str() + startPos;


        startPos = szFileData.find_first_of(szDelimiters, startPos, szDelimitersLen);
        startPos = szFileData.find_first_not_of(szDelimiters, startPos, szDelimitersLen);

        bool bError = false;

        uint32 uSeason;
        if (szLine[0] == '#')
        {
            continue;
        }
        else if (sscanf(szLine, "[Season %u]", &uSeason) == 1)
        {
            // A season seen before starts over.


            // no row of its own: the original clears it in the loop preheader (mov at 3170:42)
            ptSeasonInfo = NULL;
            for (SeasonInfo & tSeasonInfo : satSeasons)
            {
                if (tSeasonInfo.uSeasonNum == uSeason)
                {
                    ptSeasonInfo = &tSeasonInfo;  // no row of its own (the mov sits at 3172:21); placement inferred

                    ptSeasonInfo->tPreSeasonStartTime = 0; ptSeasonInfo->tStartTime = 0; ptSeasonInfo->tEndTime = 0;
                }
            }



            if (ptSeasonInfo == NULL)
            {
                ptSeasonInfo = &satSeasons.push_back();
                ptSeasonInfo->uSeasonNum = uSeason;
            }
        }
        else if (ptSeasonInfo != NULL)
        {
            char szKey[128] = {};
            char szValue[128] = {};
            if (sscanf(szLine, "%127s \"%127[^\"]\"", szKey, szValue) == 2)
                if (!strcmp(szKey, "Pre"))          ptSeasonInfo->tPreSeasonStartTime = Blizzard::Time::FromHTTPDate(szValue);
                else if (!strcmp(szKey, "Start"))   ptSeasonInfo->tStartTime = Blizzard::Time::FromHTTPDate(szValue);
                else if (!strcmp(szKey, "End"))     ptSeasonInfo->tEndTime = Blizzard::Time::FromHTTPDate(szValue);
                else bError = true;
            else
            {
                bError = true;
            }
        }
        else
        {
            bError = true;
        }

        if (bError)
            break;

    }
    blz::sort(satSeasons.begin(), satSeasons.end(),
        [](const SeasonInfo & a, const SeasonInfo & b)
        {
            return a.uSeasonNum < b.uSeasonNum;
        });
    sEvaluateSeasonState(false);
}





void LobbyServiceInternal::OnBlacklistFileRetrieved(StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
{
    // The blacklist file: a "[GBID]" section of "name,allowDrop" lines and a "[SNO]" section
    // of "group,name,allowSpawn" lines. Lines starting with '#' are comments.
    sbBlacklistDownloadPending = false;
    // (3227-3239)












    if (eResult != STORAGE_SUCCESS)
    {
        if (eResult == STORAGE_FAILED_FILE_NOT_FOUND)
        {
            // No file means nothing is blacklisted.

            sbBlacklistDataReceived = true;
        }
        return;
    }

    sbBlacklistDataReceived = true;

    bool GBIDBlock = false;
    bool SNOBlock = false;
    bool bError = false;


    blz::string & szFileData = *pszFileData;
    const char * szDelimiters = "\r\n";
    size_t szDelimitersLen = strlen(szDelimiters);


    // (3259)
    tBlacklistedGBIDs.clear();
    tBlacklistedSNOs.clear();



    size_t startPos = szFileData.find_first_not_of(szDelimiters, 0, szDelimitersLen);
    while (startPos != blz::string::npos)
    {
        const char * szLine = szFileData.c_str() + startPos;


        size_t endPos = szFileData.find_first_of(szDelimiters, startPos, szDelimitersLen);
        size_t lineLen = endPos - startPos;
        startPos = szFileData.find_first_not_of(szDelimiters, endPos, szDelimitersLen);


        if (szLine[0] == '#' || szLine[0] == '\r' || szLine[0] == '\n')
        {
            continue;
        }
        uint32 uSeason;
        if (strncmp(szLine, "[GBID]", lineLen) == 0)
        {
            GBIDBlock = true;
            SNOBlock = false;
        }
        else if (strncmp(szLine, "[SNO]", lineLen) == 0)
        {
            SNOBlock = true;
            GBIDBlock = false;
        }
        else if (GBIDBlock)
        {
            char gbidName[128];
            uint32 allowDrop;
            memset(gbidName, 0, sizeof(gbidName));
            if (sscanf(szLine, "%127[^,],%u", gbidName, &allowDrop) == 2)
            {
                // (3296-3303)







                uint32 flags = allowDrop == 0 ? 1 : 0;
                GBID gbidItem = GBIDExists(gbidName, GB_ITEMS);
                if (gbidItem != -1)
                {
                    tBlacklistedGBIDs.insert(GBIDBlacklistMap::value_type(gbidItem, flags));
                }
            }
            else
            {
                bError = true;
            }
        }
        else if (SNOBlock)
        {
            char snoName[128];
            uint32 allowSpawn;
            char snoGroupName[128];
            memset(snoName, 0, sizeof(snoName));
            memset(snoGroupName, 0, sizeof(snoGroupName));
            if (sscanf(szLine, "%127[^,],%127[^,],%u", snoGroupName, snoName, &allowSpawn) == 3)
            {
                // (3326-3335)









                uint32 flags = allowSpawn == 0 ? 4 : 0;
                SNOGroup snoGroup = SNOGroupGetByName(snoGroupName);
                uint32 sno = SNOFileLookupGUID(snoGroup, snoName);
                if (snoGroup != -1 && sno != (uint32)-1)
                {
                    // (3340-3349)









                    SNOName sno_name(snoGroup, sno);
                    tBlacklistedSNOs.insert(SNOBlacklistMap::value_type(sno_name, flags));
                }
            }
            else
            {
                bError = true;
            }
        }
        else
        {
            bError = true;
        }

        if (bError)
        {
            break;
        }
    }
}
void LobbyServiceInternal::OnUpdateInfoRetrieved(
    StorageResult eResult,
    blz::shared_ptr<blz::string> pszFileData)
{
    if (eResult != STORAGE_SUCCESS)
    {
        return;
    }







    // DWARF location list proves the initial required version is zero.
    int32 nTitleVersionRequired = 0;

    UpdateFileInfo & tUpdateInfo = m_atUpdateFiles[FILE_UPDATES_GLOBAL];
    UpdateFileInfo & tUpdateLocaleInfo = m_atUpdateFiles[FILE_UPDATES_LOCALE];



    KeyValueMap mapKeyValues;
    Prefs::ParseFileDataToKeyValues(pszFileData->modify_data(), pszFileData->size(), mapKeyValues);


    CRefString sValue;
    if (mapKeyValues.Lookup("TitleVersion", sValue))
    {
        nTitleVersionRequired = atoi(sValue);
    }

    if (nTitleVersionRequired != TITLE_VERSION_CURRENT)
    {
        return;
    }



    if (mapKeyValues.Lookup("Size", sValue))
    {
        tUpdateInfo.m_uFileSize = atoi(sValue);
    }
    if (mapKeyValues.Lookup("TOC", sValue))
    {
        tUpdateInfo.m_szTOCFilename = sValue;
    }


    const char * szLocale = XLocaleToString(UnicodeText::GetCurrentLocale());
    if (mapKeyValues.Lookup(CRefString().Format("%s-Size", szLocale), sValue))
    {
        tUpdateLocaleInfo.m_uFileSize = atoi(sValue);
    }
    if (mapKeyValues.Lookup(CRefString().Format("%s-TOC", szLocale), sValue))
    {
        tUpdateLocaleInfo.m_szTOCFilename = sValue;
    }





    // Inference: empty failure branches fall through to the common clear loop at 3485.
    if (tUpdateInfo.m_szTOCFilename.empty())
    {
        // Inference: the next original comparison rejects sizes above 0x100000 bytes.
    }
    else if (tUpdateInfo.m_uFileSize > 0x100000)
    {


    }



    else
    {
        tUpdateInfo.m_szBuffer.reserve(tUpdateInfo.m_uFileSize);

        if (tUpdateLocaleInfo.m_uFileSize == 0)
        {
            tUpdateLocaleInfo.Clear();
        }
        else if (tUpdateLocaleInfo.m_szTOCFilename.empty())
        {
            tUpdateLocaleInfo.Clear();
        }
        // Inference: the original uses the same 0x100000-byte limit for the locale file.
        else if (tUpdateLocaleInfo.m_uFileSize > 0x100000)
        {
            tUpdateLocaleInfo.Clear();
        }
        else
        {

            tUpdateLocaleInfo.m_szBuffer.reserve(tUpdateLocaleInfo.m_uFileSize);
        }
        // Inference: successful dispatch returns; failure paths reach the common clear loop.
        OnUpdateFragmentRetrieved(NULL, 0, STORAGE_SUCCESS, nullptr);
        return;
    }





    for (int32 i = 0; i < NUM_FILE_TYPES; ++i)
    {
        m_atUpdateFiles[i].Clear();
    }
}






// X/Client/Console/UpdateManager.cpp:391 (not in src/ yet).
}  namespace UpdateManager { void OnNewUpdateDownloaded(int32 nUpdateVersion, const char * szTOC, blz::string szUpdate, const char * szLocaleTOC, blz::string szLocaleUpdate); }  namespace Online {
void LobbyServiceInternal::OnUpdateFragmentRetrieved(UpdateFileInfo * ptUpdateInfo, uint32 uFragmentNum, StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
{
    switch (eResult)
    {
    case STORAGE_FAILED_NOT_CONNECTED:
        for (uint32 i = 0; i < NUM_FILE_TYPES; ++i)
            m_atUpdateFiles[i].Clear();
        m_nFileInProgress = FILE_NONE;
        return;





    default:
        // A fragment is missing: drop this file, and give up if it was the global one.

        ptUpdateInfo->Clear();


        if (m_nFileInProgress == FILE_UPDATES_GLOBAL)
        {
            m_nFileInProgress = FILE_NONE;
            return;
        }
        ptUpdateInfo = NULL;
        break;
    case STORAGE_SUCCESS:
        if (ptUpdateInfo != NULL)
        {

            ptUpdateInfo->m_szBuffer.append(*pszFileData);






            ++uFragmentNum;

            if (ptUpdateInfo->m_szBuffer.size() == ptUpdateInfo->m_uFileSize)
            {
                ptUpdateInfo = NULL;
            }

            else if (ptUpdateInfo->m_szBuffer.size() > ptUpdateInfo->m_uFileSize)
            {

                ptUpdateInfo->Clear();
                ptUpdateInfo = NULL;
            }
        }
        break;
    }




    if (ptUpdateInfo == NULL)
    {
        for (++m_nFileInProgress; (uint32)m_nFileInProgress < NUM_FILE_TYPES; ++m_nFileInProgress)
        {
            if (m_atUpdateFiles[m_nFileInProgress].m_uFileSize != 0)
            {
                ptUpdateInfo = &m_atUpdateFiles[m_nFileInProgress];
                uFragmentNum = 1;
                break;
            }
        }







        if (ptUpdateInfo == NULL)
        {

            m_nFileInProgress = FILE_NONE;

            UpdateFileInfo & tUpdate = m_atUpdateFiles[FILE_UPDATES_GLOBAL];
            UpdateFileInfo & tUpdateLocale = m_atUpdateFiles[FILE_UPDATES_LOCALE];
            UpdateManager::OnNewUpdateDownloaded(
                m_nBalanceVersion,
                tUpdate.m_szTOCFilename.c_str(),
                blz::move(tUpdate.m_szBuffer),
                tUpdateLocale.m_szTOCFilename.c_str(),
                blz::move(tUpdateLocale.m_szBuffer));

            tUpdate.Clear();
            tUpdateLocale.Clear();
            return;
        }
    }

    char szFilename[128];
    Blizzard::String::Format(szFilename, sizeof(szFilename), "%s.%d", ptUpdateInfo->m_szFilename.c_str(), uFragmentNum);


    StorageGetPublisherFile(szFilename,

        StorageGetFileCallback(OnlineService::internal::method_bind_2_2<void, LobbyServiceInternal, LobbyServiceInternal *, UpdateFileInfo *, uint32, StorageResult, blz::shared_ptr<blz::string>, &LobbyServiceInternal::OnUpdateFragmentRetrieved>(this, ptUpdateInfo, uFragmentNum)), false);
}




void LobbyServiceInternal::OnServerTime(BOOL bSuccess, const bdTimeStamp & tTimestamp)
{
    // (3633-3635)
    if (bSuccess)
    {
        // (3637)
        Blizzard::Time::Timestamp tNow = Blizzard::Time::GetTimestamp();
        stTimeOffset = Blizzard::Time::FromUnixTime64(tTimestamp.m_timeStamp) - tNow;

        sEvaluateSeasonState(true);
    }
}




void LobbyServiceInternal::onNewMail()
{
    // (3649-3650)
    m_tNewMailEvent.NotifySubscribers();
}










void LobbyServiceInternal::onSubscribedRichPresenceUpdate(const bdNChar8 * szContext, const bdTitleID idTitle, const bdRichPresenceSource tSource, const bdRichPresenceData & tData)
{
    // (3674-3678)



    if (idTitle == m_idTitle)
    {
        // (3680-3681)

        PresenceManager::Get(m_nUserIndex)->OnRichPresenceUpdateReceived(tData);
    }
}

void sReceivedPartyInvitation(blz::string szSenderName, PartyConnectionInfo tConnectionInfo, int32 nUserIndex);  // Online.cpp:3321

// NON_MATCHING: 704 vs 708 B; a failed readBits in the inlined readUInt32(uMessageType) is jump-threaded past the uMessageType test (ours: undef when !ok), while the original loads nu unconditionally and keeps bOk in w22; likely bdBitOperations::endianSwap / bdBitBuffer::readUInt32 header bodies (invented) differ, not this function
void LobbyServiceInternal::onGlobalInstantMessage(
    bdOnlineUserID idSender,
    bdNChar8 * szSenderName,
    void * ptMessage,
    bdUInt uMessageSize)
{
    // Party invitations arrive as global instant messages: a version, a message type and,
    // for an invitation, the party's connection info.


    if (ptMessage == NULL || uMessageSize == 0)
    {
        return;
    }

    bdBitBufferRef ptBitBuffer(::new bdBitBuffer((const bdUByte8 *)ptMessage, uMessageSize * 8, true));

    uint32 uVersion;
    bdBool bOk = ptBitBuffer->readUInt32(uVersion);
    // bOk declared with the uVersion read (DWARF decl 3708) and overwritten by the uMessageType read (3712); uMessageType declared at 3711

    uint32 uMessageType;
    bOk = ptBitBuffer->readUInt32(uMessageType);


    if (uMessageType == 1)
    {
        PartyConnectionInfo tConnectionInfo;
        bOk = bOk && tConnectionInfo.Deserialize(ptBitBuffer);

        if (bOk)
        {


            QueueNetworkEventForMainThread(OnlineService::internal::function_binder_3<void, blz::string, PartyConnectionInfo, int32>::bind<&sReceivedPartyInvitation>(blz::string(szSenderName), tConnectionInfo, m_nUserIndex));
        }
    }
}



void LobbyServiceInternal::onMultipleLogon(bdOnlineUserID userID)
{
    if (userID == m_idOnlineUser)
    {
        m_bFatalErrorOccurred = true;
        // The same account signed in elsewhere: the main thread drops this user.

        QueueNetworkEventForMainThread(OnlineService::internal::function_binder_1<void, int>::bind<&sHandleMultipleLogon>(m_nUserIndex));
    }
}






inline void LobbyServiceInternal::OnDisconnect()
{
    if (m_bWasConnected)
    {
        m_bWasConnected = false;
        --ms_nNumLobbyServicesConnected;
    }
    m_tDisconnectEvent.NotifySubscribers(m_nUserIndex);
}




LobbyTask::LobbyTask(LobbyService * ptLobbyService)
    // (3761)
    : Task(ptLobbyService->GetTaskManager())
    , m_ptLobbyService(ptLobbyService)
{
}




void LobbyTask::Cancel()
{
    if (m_ptRemoteTask &&
        m_ptRemoteTask->getStatus() == bdRemoteTask::BD_PENDING)
    {

        m_ptRemoteTask->cancelTask();
    }

    m_ptRemoteTask = NULL;


    Task::Cancel();

    m_ptLobbyService->GetTaskManager().RemoveTask(this);
}


Task::Status LobbyTask::GetStatus()
{
    if (m_ptRemoteTask.isNull())
    {
        // Not started yet: it can start once the lobby is connected.


        if (m_ptLobbyService->GetLobbyService())
            return m_ptLobbyService->GetLobbyService()->getStatus() == bdLobbyConnection::BD_CONNECTED ? TASK_NOT_STARTED : TASK_NOT_STARTED_CANT_START;

        return TASK_NOT_STARTED_CANT_START;
    }

    // (3799-3803)



    bdRemoteTask::bdStatus eStatus = m_ptRemoteTask->getStatus();
    return eStatus == bdRemoteTask::BD_PENDING ? TASK_IN_PROGRESS : (eStatus == bdRemoteTask::BD_DONE ? TASK_DONE : TASK_FAILED);
}













BOOL LobbyIsConnected(int32 nUserIndex)
{
    // (3822-3823)
    return LobbyService::Get(nUserIndex)->IsConnected();
}




void StorageGetPublisherFile(int32 nUserIndex, const char * szFilename, StorageGetFileCallback pfnCallback)
{
    // The answer reaches the caller on the main thread.
    // (3833-3834)

    ScopedNetworkThreadLock tLock;

    // (3837)
    LobbyService::Get(nUserIndex)->StorageGetPublisherFile(szFilename,
        [pfnCallback](StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
        {
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, StorageGetFileCallback>, const StorageResult &, const blz::shared_ptr<blz::string> &>::bind<&OnlineService::internal::callback_base<2, StorageGetFileCallback>::execute>(pfnCallback, eResult, pszFileData));
        },
        false);
}



void StorageFindUserFile(int32 nUserIndex, const char * szContext, const char * szFilename, StorageFindUserFileCallback pfnCallback, bdUserAccountID idUser)
{
    // (3850-3854)




    ScopedNetworkThreadLock tLock;

    // (3857)
    LobbyService::Get(nUserIndex)->StorageFindUserFile(idUser, szContext, szFilename,
        [pfnCallback](StorageResult eResult, const bdUserStorageFileInfo & tFileInfo)
        {
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, StorageFindUserFileCallback>, const StorageResult &, const bdUserStorageFileInfo &>::bind<&OnlineService::internal::callback_base<2, StorageFindUserFileCallback>::execute>(pfnCallback, eResult, tFileInfo));
        });
}




void StorageGetUserFile(int32 nUserIndex, const char * szContext, const bdUserStorageFileInfo & tFileInfo, StorageGetFileCallback pfnCallback)
{
    // (3870-3873)



    ScopedNetworkThreadLock tLock;

    // (3876-3877)

    LobbyService::Get(nUserIndex)->StorageGetUserFile(szContext, tFileInfo,
        [pfnCallback](StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
        {
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, StorageGetFileCallback>, const StorageResult &, const blz::shared_ptr<blz::string> &>::bind<&OnlineService::internal::callback_base<2, StorageGetFileCallback>::execute>(pfnCallback, eResult, pszFileData));
        });
}




void StoragePublishUserFile(int32 nUserIndex, const char * szContext, const char * szFilename, blz::string szData, StoragePublishUserFileCallback pfnCallback)
{
    // (3890-3894)




    ScopedNetworkThreadLock tLock;

    LobbyService::Get(nUserIndex)->StoragePublishUserFile(szContext, szFilename, blz::move(szData),
        [pfnCallback](StorageResult eResult)
        {
            if (pfnCallback)
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, StoragePublishUserFileCallback>, const StorageResult &>::bind<&OnlineService::internal::callback_base<1, StoragePublishUserFileCallback>::execute>(pfnCallback, eResult));
        });
}















LeaderboardStatsInfo & LeaderboardScoreToStat(LeaderboardStatsInfo & tStat, const D3::Leaderboard::PostScore & tScore, int64 nRating, bool bMetadataLB)
{
    // (3920)
    tStat.m_leaderboardID = LeaderboardToDemonwareLeaderboardId(tScore.category_id(), (LeaderboardScope)tScore.scope_id(), tScore.partition_id(), bMetadataLB);
    tStat.m_rating = nRating;
    // (3923)
    tStat.m_writeType = bdStats::STAT_WRITE_REPLACE_WHEN_RATING_INCREASE;

    // (3926)
    tStat.m_score = tScore.score();
    tStat.m_timestamp = tScore.timestamp();
    tStat.m_metadata = tScore.metadata();

    if (!bMetadataLB)
    {
        // (3933)
        if (!LeaderboardIsChallengeRift(tScore.category_id()))
        {
            while (tStat.m_metadata.team_member_size() > 1)
            {
                tStat.m_metadata.mutable_team_member()->RemoveLast();
            }
        }

        // (3942)
        for (D3::Leaderboard::Member & tMember : *tStat.m_metadata.mutable_team_member())
        {
            tMember.clear_hero_snapshot();
        }
    }

    // (3948)
    return tStat;
}



extern "C" bool sub_7100517f80(LeaderboardScope eScope);

void LeaderboardPostRiftTime(int32 nUserIndex, const D3::Leaderboard::PostScore & tScoreFromServer, LeaderboardPostCallback pfnCallback)
{
    // (3958)
    D3::Leaderboard::PostScore tScore(tScoreFromServer);

    // (3961-3968)







    Blizzard::Time::Timestamp stamp = (GetNetworkTimestamp() / 10000000) * 10000000;

    // (3971)
    tScore.set_score(LeaderboardMakeRiftScore(
        LeaderboardGetRiftScoreLevel(tScore.score()),
        (int32)(LeaderboardGetRiftScoreTimeToComplete(tScore.score()) / 1000000),
        Blizzard::Time::ToUnixTime(stamp)));
    tScore.set_timestamp(stamp);

    // (3978-3982)




    if (!GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    // (3987)
    if (LeaderboardIsRift(tScore.category_id()))
    {
        if (OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE ||
            OnlineService::GetSeasonNum() != tScore.partition_id() ||
            OnlineService::GetSeasonNum() != GamerProfile::GetAccount(nUserIndex).digest().season_id())
        {
            return;
        }
    }
    // (3998)
    else if (LeaderboardIsChallengeRift(tScore.category_id()))
    {
        // (4001)
        GameParams tParams;
        if (!PartyGetGameParams(tParams) ||
            tParams.m_eGameType != 6 ||
            tScore.partition_id() != tParams.m_tWeeklyChallengeConfig.challenge_number() ||
            GetNetworkTimestamp() >= Blizzard::Time::FromUnixTime64(tParams.m_tWeeklyChallengeConfig.challenge_end_unix_time_console()))
        {
            return;
        }
    }
    else
    {
        return;
    }

    // (4015-4018)



    int64 nRating = LeaderboardMakeRiftDemonwareRating(tScore.score());

    // (4021-4022)

    blz::vector<LeaderboardStatsInfo> tStats;
    tStats.reserve(3);

    // (4025)
    LeaderboardStatsInfo & tStat = LeaderboardScoreToStat(tStats.push_back(), tScore, nRating, false);
    if (LeaderboardIsRift(tScore.category_id()))
    {
        LeaderboardScoreToStat(tStats.push_back(), tScore, nRating, true);

        // (4031)
        if ((tScore.category_id() < 10 || tScore.category_id() > 12) &&
            !sub_7100517f80((LeaderboardScope)tScore.scope_id()))
        {
            // (4034-4035)

            LeaderboardStatsInfo & tStatCopy = tStats.push_back();
            tStatCopy = tStat;
            // (4037)
            tStatCopy.m_leaderboardID = LeaderboardToDemonwareLeaderboardId(13, (LeaderboardScope)tScore.scope_id(), tScore.partition_id(), false);
        }
    }

    // (4041)
    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardWriteStats(tStats,
        // (4044)
        [pfnCallback](LeaderboardResult eResult)
        {
            if (pfnCallback)
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LeaderboardPostCallback>, const LeaderboardResult &>::bind<&OnlineService::internal::callback_base<1, LeaderboardPostCallback>::execute>(pfnCallback, eResult));
        });
}












void LeaderboardPostGenericScore(int32 nUserIndex, uint64 idLeaderboard, uint64 uScore, int64 nRating, LeaderboardPostCallback pfnCallback)
{
    bdLeaderBoardID idDWLeaderboard = LeaderboardToDemonwareLeaderboardId(idLeaderboard, LP_SEASONAL, OnlineService::GetSeasonNum(), false);

    if (!GamerProfile::IsActive(nUserIndex) ||
        OnlineService::GetSeasonNum() != GamerProfile::GetAccount(nUserIndex).digest().season_id() ||
        OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE)
    {
        return;
    }

    // (4078-4091)













    blz::vector<LeaderboardStatsInfo> tStats;
    LeaderboardStatsInfo & tStat = tStats.push_back();

    tStat.m_leaderboardID = idDWLeaderboard;
    // (4095)
    tStat.m_writeType = bdStats::STAT_WRITE_REPLACE_WHEN_RATING_INCREASE;
    tStat.m_rating = nRating;
    tStat.m_score = uScore;
    tStat.m_timestamp = GetNetworkTimestamp();
    // (4100)
    Player * ptPlayer = PlayerGetByControllerIndex(nUserIndex);
    if (!ptPlayer)
    {
        // (4104-4107)



        if (pfnCallback)
        {
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LeaderboardPostCallback>, const LeaderboardResult &>::bind<&OnlineService::internal::callback_base<1, LeaderboardPostCallback>::execute>(pfnCallback, LeaderboardResult::FAILED));
        }
        return;
    }

    // (4114-4115)

    SLeaderboard::FillInMemberMetadata(tStat.m_metadata.add_team_member(), ptPlayer, false);

    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardWriteStats(tStats,
        [pfnCallback](LeaderboardResult eResult)
        {
            if (pfnCallback)
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LeaderboardPostCallback>, const LeaderboardResult &>::bind<&OnlineService::internal::callback_base<1, LeaderboardPostCallback>::execute>(pfnCallback, eResult));
        });
}










void LeaderboardPostAchievementScore(int32 nUserIndex, uint32 uAchievementPoints, LeaderboardPostCallback pfnCallback)
{
    if (!GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    // (4145)
    D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nUserIndex);
    if (tAccount.console_data().leaderboard_heal_achievements() < uAchievementPoints)
    {
        tAccount.mutable_console_data()->set_leaderboard_heal_achievements(uAchievementPoints);
    }

    // (4151)
    uint64 uScore = LeaderboardMakeAchievementPointScore(uAchievementPoints, Blizzard::Time::ToUnixTime(GetNetworkTimestamp()));
    LeaderboardPostGenericScore(nUserIndex, 1, uScore,
        // (4154-4156)


        LeaderboardMakeAchievementDemonwareRating(uScore),
        [nUserIndex, uAchievementPoints, pfnCallback](LeaderboardResult eResult)
        {
            if (eResult == LeaderboardResult::SUCCESS &&
                GamerProfile::IsActive(nUserIndex))
            {
                // (4162)
                D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nUserIndex);
                if (uAchievementPoints >= tAccount.console_data().leaderboard_heal_achievements())
                {
                    tAccount.mutable_console_data()->clear_leaderboard_heal_achievements();
                }
            }

            // (4169)
            if (pfnCallback)
                pfnCallback(eResult);
        });
}






void LeaderboardPostConquest(int32 nUserIndex, uint64 idConquest, LeaderboardPostCallback pfnCallback)
{
    if (!GamerProfile::IsActive(nUserIndex))
    {
        return;
    }

    // (4190)
    D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nUserIndex);
    const google::protobuf::RepeatedField<uint64> & aConquests = tAccount.console_data().leaderboard_heal_conquests();
    if (blz::find(aConquests.begin(), aConquests.end(), idConquest) == aConquests.end())
    {
        tAccount.mutable_console_data()->add_leaderboard_heal_conquests(idConquest);
    }

    // (4197)
    uint64 uScore = LeaderboardMakeConquestScore(GetNetworkTimestamp());
    LeaderboardPostGenericScore(nUserIndex, idConquest, uScore,
        // (4200-4202)

        LeaderboardMakeConquestDemonwareRating(uScore),
        [nUserIndex, idConquest, pfnCallback](LeaderboardResult eResult)
        {
            if (eResult == LeaderboardResult::SUCCESS &&
                GamerProfile::IsActive(nUserIndex))
            {
                // (4209-4211)
                D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(nUserIndex);

                const google::protobuf::RepeatedField<uint64> & conquestsToHeal = tAccountDefinition.console_data().leaderboard_heal_conquests();
                if (blz::find(conquestsToHeal.begin(), conquestsToHeal.end(), idConquest) != conquestsToHeal.end())
                {
                    for (int32 i = 0; i < conquestsToHeal.size(); ++i)
                    {
                        if (tAccountDefinition.console_data().leaderboard_heal_conquests(i) == idConquest)
                        {
                            tAccountDefinition.mutable_console_data()->mutable_leaderboard_heal_conquests()->ExtractSubrange(i, 1, NULL);
                            break;
                        }
                    }

                    if (tAccountDefinition.console_data().leaderboard_heal_conquests_size() == 0)
                    {
                        tAccountDefinition.mutable_console_data()->clear_leaderboard_heal_conquests();
                    }
                }
            }

            if (pfnCallback)
            {
                pfnCallback(eResult);
            }
        });
}




void LeaderboardPerformHealing(int32 nUserIndex)
{
    // (4242-4243)

    DeferNetworkEventForMainThread(5000,
        [nUserIndex]()
        {
            // (4246-4247)

            if (!GamerProfile::IsActive(nUserIndex) ||
                GameIsStartUp())
            {
                return;
            }

            // (4252-4253)

            D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nUserIndex);

            // (4256)
            if (tAccount.console_data().leaderboard_heal_achievements() == 0 &&
                tAccount.console_data().leaderboard_heal_conquests_size() == 0)
            {
                return;
            }

            // (4262-4263)

            if (!LobbyService::Get(nUserIndex)->IsConnected() || !sbSeasonsInitialized)
            {
                return;
            }

            // (4269-4270)

            if (tAccount.digest().season_id() != OnlineService::GetSeasonNum() ||
                OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE)
            {
                // (4274-4275)

                tAccount.mutable_console_data()->clear_leaderboard_heal_conquests();
                tAccount.mutable_console_data()->clear_leaderboard_heal_achievements();
                return;
            }

            Player * ptPlayer = PlayerGetByControllerIndex(nUserIndex);
            if (!ptPlayer)
            {
                return;
            }

            // (4285-4286)

            if (!ACDTryToGet(ptPlayer->idPlayerACD))
            {
                LeaderboardPerformHealing(nUserIndex);
                return;
            }

            // (4292)
            for (uint64 idConquest : tAccount.console_data().leaderboard_heal_conquests())
            {
                // (4294)
                LeaderboardPostConquest(nUserIndex, idConquest, LeaderboardPostCallback());
            }

            // (4297)
            if (tAccount.console_data().leaderboard_heal_achievements() != 0)
            {
                // (4299)
                LeaderboardPostAchievementScore(nUserIndex, tAccount.console_data().leaderboard_heal_achievements(), LeaderboardPostCallback());
            }
        });
}
void ProcessLeaderboardResults_OnGetProfileNames(LeaderboardGetScoresCallback pfnCallback, blz::vector<D3::Leaderboard::Score> atSlots, BOOL bSuccess, const PresenceProfileNameMap & tResults)
{
    if (bSuccess)
    {
        for (D3::Leaderboard::Score & tSlot : atSlots)
        {
            for (D3::Leaderboard::Member & tMember : *tSlot.mutable_metadata()->mutable_team_member())
            {
                // Fill in the battle tags the presence lookup found.






                PresenceProfileNameMap::const_iterator it = tResults.find(tMember.account_id());
                if (it != tResults.end())
                {
                    tMember.set_battle_tag(it->second);
                }
            }
        }
    }


    QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>, const LeaderboardResult &, const blz::vector<D3::Leaderboard::Score> &>::bind<&OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>::execute>(pfnCallback, LeaderboardResult::SUCCESS, atSlots));
}


void PresenceGetProfileNames(int32 nUserIndex, const blz::vector<OnlineUserId> & idUsers, PresenceGetProfileNamesCallback pfnCallback);  // PresenceManager.cpp:1391


void ProcessLeaderboardResults(int32 nUserIndex, LeaderboardGetScoresCallback pfnCallback, LeaderboardResult eResult, const blz::vector<LeaderboardStatsInfo> & atResults)
{

    if (!pfnCallback)
    {
        return;
    }
    // atScores declared before the error exit: the original passes it at 4359 (no temporary vector there) and destroys it on that return
    blz::vector<D3::Leaderboard::Score> atScores;

    if (eResult != LeaderboardResult::SUCCESS)
    {
        QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>, const LeaderboardResult &, const blz::vector<D3::Leaderboard::Score> &>::bind<&OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>::execute>(pfnCallback, eResult, atScores));
        return;
    }

    if (atResults.size() > 0)
    {
        atScores.reserve(atResults.size());

        for (const LeaderboardStatsInfo & tStat : atResults)
        {
            if (tStat.m_lbKey.m_id == 0 ||
                tStat.m_rating == 0)
            {
                continue;
            }
            // continue form inferred: the original line table puts the 4378-4390 statements one indent level above the 4370 condition (columns 54/61/60)

            D3::Leaderboard::Score & tScore = atScores.push_back();

            tScore.set_game_account_id(tStat.m_entityID);
            tScore.set_leaderboard_id(tStat.m_lbKey.m_id);
            tScore.set_scope_id(tStat.m_lbKey.m_scope);

            tScore.set_score(tStat.m_score);
            tScore.set_score_band((uint32)tStat.m_rank);
            tScore.set_timestamp(tStat.m_timestamp);
            tScore.mutable_metadata()->CopyFrom(tStat.m_metadata);

            if (tScore.metadata().team_member_size() == 0)
            {
                D3::Leaderboard::Member * ptTeamMember = tScore.mutable_metadata()->add_team_member();
                ptTeamMember->set_account_id(tStat.m_entityID);
            }

        }
    }

    blz::set<unsigned long> idUsers;
    bdOnlineUserID idOnlineUserLocal = LobbyService::Get(nUserIndex)->GetOnlineUserID();

    for (D3::Leaderboard::Score & tScore : atScores)
    {
        for (D3::Leaderboard::Member & tMember : *tScore.mutable_metadata()->mutable_team_member())
        {
            if (tMember.account_id() != 0)
            {
                if (idOnlineUserLocal != 0 && tMember.account_id() == idOnlineUserLocal)
                {
                    // (4407) column 52 of 4405 is this && (original line table); the original unswitches the loop on idOnlineUserLocal != 0
                    tMember.set_battle_tag(GamerProfile::GetGamerTag(nUserIndex));
                    continue;
                }
                // continue form inferred: 4412 sits at the 4405 indent level (column 44), 4408 one level deeper (columns 37/52)
                Friend * ptFriend = PresenceManager::Get(nUserIndex)->GetFriend(tMember.account_id());
                if (ptFriend != NULL &&
                    ptFriend->m_szUsername[0] != 0)
                {
                    tMember.set_battle_tag(ptFriend->m_szUsername);
                }
                else
                {
                    idUsers.insert(tMember.account_id());
                }
            }
        }
    }





    if (!idUsers.empty())
    {
        PresenceGetProfileNames(nUserIndex,
            blz::vector<unsigned long>(idUsers.begin(), idUsers.end()),
            OnlineService::internal::function_binder_4<void, LeaderboardGetScoresCallback, blz::vector<D3::Leaderboard::Score>, BOOL, const PresenceProfileNameMap &>::bind<&ProcessLeaderboardResults_OnGetProfileNames>(pfnCallback, atScores));
    }
    else

        QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>, const LeaderboardResult &, const blz::vector<D3::Leaderboard::Score> &>::bind<&OnlineService::internal::callback_base<2, LeaderboardGetScoresCallback>::execute>(pfnCallback, LeaderboardResult::SUCCESS, atScores));
}



// NON_MATCHING: two instructions swapped (the asr computing the batch vector's capacity is scheduled before the store that clears its elements pointer)
void LeaderboardGetMultipleScoresForPlayer(
    int32 nUserIndex,
    const blz::vector<LeaderboardKeyAndPartition> & idLeaderboards,
    LeaderboardGetScoresCallback pfnCallback)
{
    struct BatchResults // DWARF DW_TAG_structure_type at 4449
    {
        int32 m_nUserIndex;
        LeaderboardGetScoresCallback m_pfnCallback;
        uint32 m_uNumOutsanding;
        blz::vector<LeaderboardStatsInfo> m_atResults;
        // The leaderboards are read in batches of 20; the results are collected here until the last batch completes.
        BatchResults(
            int32 nUserIndex,
            LeaderboardGetScoresCallback pfnCallback)
            : m_nUserIndex(nUserIndex)
            , m_pfnCallback(pfnCallback)
            , m_uNumOutsanding(0)
        {
        }
        void Start(
            const blz::vector<LeaderboardKeyAndPartition> & idLeaderboards)
        {
            m_atResults.reserve(idLeaderboards.size());

            {
                ScopedNetworkThreadLock tLock;
                for (size_t uStart = 0; uStart < idLeaderboards.size(); )
                {
                    size_t uEnd = idLeaderboards.size() < uStart + 20 ? idLeaderboards.size() : uStart + 20;

                    ++m_uNumOutsanding;
                    LobbyService::Get(m_nUserIndex)->LeaderboardReadStatsByLeaderboardIds(
                        blz::vector<LeaderboardKeyAndPartition>(
                            idLeaderboards.begin() + uStart,
                            idLeaderboards.begin() + uEnd),
                        LobbyService::LeaderboardReadStatsCallback(OnlineService::internal::method_bind_0_2<void, BatchResults, BatchResults *, LeaderboardResult, const blz::vector<LeaderboardStatsInfo> &, &BatchResults::OnResults>(this)));

                    uStart = uEnd;
                }
            }

            if (m_uNumOutsanding == 0)
            {
                Finish(LeaderboardResult::SUCCESS);
            }
        }

        void OnResults(
            LeaderboardResult eResult,
            const blz::vector<LeaderboardStatsInfo> & atStats)
        {
            if (eResult == LeaderboardResult::SUCCESS && atStats.size() != 0)
            {
                m_atResults.insert(m_atResults.end(), atStats.begin(), atStats.end());
            }


            if (--m_uNumOutsanding == 0)
            {
                Finish(eResult);
            }
        }

        void Finish(
            LeaderboardResult eResult)
        {
            ProcessLeaderboardResults(m_nUserIndex, m_pfnCallback, eResult, m_atResults);
            delete this;
        }
    };

    BatchResults * ptBatcher = new BatchResults(nUserIndex, pfnCallback);
    ptBatcher->Start(idLeaderboards);
}







void LeaderboardGetScoresForUsers(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<unsigned long> & idUsers, LeaderboardGetScoresCallback pfnCallback, bool bIncludeSelf)
{
    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardReadStatsByEntityId(idLeaderboard, idUsers, bIncludeSelf,
        OnlineService::internal::function_binder_4<void, int32, LeaderboardGetScoresCallback, LeaderboardResult, const blz::vector<LeaderboardStatsInfo> &>::bind<&ProcessLeaderboardResults>(nUserIndex, pfnCallback));




}





void LeaderboardGetScoresAroundPlayer(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, uint32 uMaxResults, LeaderboardGetScoresCallback pfnCallback)
{
    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardReadStatsByPivot(idLeaderboard, uMaxResults,


        OnlineService::internal::function_binder_4<void, int32, LeaderboardGetScoresCallback, LeaderboardResult, const blz::vector<LeaderboardStatsInfo> &>::bind<&ProcessLeaderboardResults>(nUserIndex, pfnCallback));
}







void LeaderboardGetScoresByRange(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, uint32 uFirstRank, uint32 uMaxResults, LeaderboardGetScoresCallback pfnCallback)
{
    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardReadStatsByRank(idLeaderboard, uFirstRank, uMaxResults,


        OnlineService::internal::function_binder_4<void, int32, LeaderboardGetScoresCallback, LeaderboardResult, const blz::vector<LeaderboardStatsInfo> &>::bind<&ProcessLeaderboardResults>(nUserIndex, pfnCallback));


}





void LeaderboardGetMetadataForUser(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, bdOnlineUserID idUser, LeaderboardGetScoresCallback pfnCallback)
{
    ScopedNetworkThreadLock tLock;
    LobbyService::Get(nUserIndex)->LeaderboardReadMetadata(idLeaderboard, idUser,
        OnlineService::internal::function_binder_4<void, int32, LeaderboardGetScoresCallback, LeaderboardResult, const blz::vector<LeaderboardStatsInfo> &>::bind<&ProcessLeaderboardResults>(nUserIndex, pfnCallback));


}




BOOL AreSeasonalGlobalLeaderboardsEnabled()
{
    // (4584-4585)
    return sbSeasonalGlobalLeaderboardsEnabled;
}






// 2.7.7: LinkedAccountsSearch is gone with LobbyService::GetLinkedAccounts.



















void GetChallengeRiftConfig(int32 nUserIndex, OnlineService::Callback<void (StorageResult, const D3::ChallengeRifts::ChallengeData &)> pfnCallback)
{
    StorageGetPublisherFile(nUserIndex, "challengerift_config.dat",
        // (4614)
        [pfnCallback](StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
        {
            D3::ChallengeRifts::ChallengeData tConfig;

            if (eResult != STORAGE_SUCCESS)
            {
                // (4621)
                pfnCallback(eResult, D3::ChallengeRifts::ChallengeData());
                return;
            }

            if (pszFileData->empty() ||
                !tConfig.ParsePartialFromString(*pszFileData) ||
                tConfig.challenge_number() == 0)
            {
                // (4629-4630)

                pfnCallback(STORAGE_FAILED_DATA_INVALID, D3::ChallengeRifts::ChallengeData());
                return;
            }

            // (4634)
            pfnCallback(STORAGE_SUCCESS, tConfig);
        });
}




void GetChallengeRiftData(int32 nUserIndex, OnlineService::Callback<void (StorageResult, const D3::ChallengeRifts::ChallengeData &, const D3::Leaderboard::WeeklyChallengeData &)> pfnCallback)
{
    GetChallengeRiftConfig(nUserIndex,
        [pfnCallback, nUserIndex](StorageResult eResult, const D3::ChallengeRifts::ChallengeData & tConfig)
        {
            if (eResult != STORAGE_SUCCESS)
            {
                pfnCallback(eResult, D3::ChallengeRifts::ChallengeData(), D3::Leaderboard::WeeklyChallengeData());
                return;
            }

            // (4655-4661)






            StorageGetPublisherFile(nUserIndex,
                blz::make_stringf("challengerift_%d.dat", tConfig.challenge_number() % 10).c_str(),
                [pfnCallback, tConfig](StorageResult eResult, blz::shared_ptr<blz::string> pszFileData)
                {
                    if (eResult != STORAGE_SUCCESS)
                    {
                        // (4668)
                        pfnCallback(eResult, tConfig, D3::Leaderboard::WeeklyChallengeData());
                        return;
                    }

                    // (4672)
                    D3::Leaderboard::WeeklyChallengeData tData;
                    if (pszFileData->empty() ||
                        !tData.ParsePartialFromString(*pszFileData) ||
                        !(tData.has_rift_snapshot() && tData.has_hero_snapshot() && tData.has_account_snapshot()))
                    {
                        // (4678-4680)


                        pfnCallback(STORAGE_FAILED_DATA_INVALID, tConfig, D3::Leaderboard::WeeklyChallengeData());
                        return;
                    }

                    // (4684)
                    if (!tData.rift_snapshot().has_completion_milliseconds())
                    {
                        tData.mutable_rift_snapshot()->set_completion_milliseconds(tData.rift_snapshot().deprecated_completion_seconds() * 1000);
                    }

                    // (4689)
                    pfnCallback(STORAGE_SUCCESS, tConfig, tData);
                });
        });
}

}  // namespace Online
}  // namespace Console
