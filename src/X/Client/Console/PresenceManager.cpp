#include "../../Common/SNO/SNOName.h"  // X/Client/Console/PresenceManager.cpp (friends list and rich presence). SNOName.h/WorldPlace.h/GfxVB.h first: the original static initializer sets SNONAME_NONE, cgplaceNull and cgc*Normal before the nn::ldn statics (DWARF variable order agrees); the original include chain is not known.
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceData.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceService.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdQoSRemoteAddr.h"
#include "../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_AsyncContext.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Friend.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Presence.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_NotificationQueue.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Profile.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "GamerProfile.h"
#include "LobbyService.h"
#include "Online.h"
#include "PresenceManager.h"
#include "TaskManager.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/LRUCache.h"
#include "../../Sigma/Random.h"
#include "../../Common/Attributes/FastAttribConstants.h"
#include "../../Common/Partition.h"
#include "../../Common/Player.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Sigma/Math/MathGeneral.h"
#include <string.h>
// GamerProfile.h / GamerProfileNX64.cpp (not in src/ yet).
namespace Console { namespace GamerProfile {
class ScopedUsersWriteLock {  // size 0x1
public:
    ScopedUsersWriteLock();
    ~ScopedUsersWriteLock();
};
nn::account::UserHandle GetUserHandle(int32 nUserIndex);  // GamerProfileNX64.cpp:718
nn::account::Uid GetUserId(int32 nUserIndex);  // GamerProfileNX64.cpp:734
class ScopedUsersReadLock {  // size 0x1
public:
    ScopedUsersReadLock();
    ~ScopedUsersReadLock();
};
} }

// Nintendo SDK entry points (nn/account/account_ApiForApplications.h, nn/friends/friends_Api.h).
namespace OnlineServiceServer { long GetCurrentUnixTime(); }  // its own header is not in src yet
namespace nn { namespace account {
nn::Result GetUserId(nn::account::Uid * pOut, const nn::account::UserHandle & handle);
} }
namespace nn { namespace friends {
nn::Result GetFriendList(int * pOutCount, nn::friends::Friend * pOutFriends, const nn::account::Uid & uid, int offset, int count, const nn::friends::FriendFilter & filter);
nn::Result UpdateFriendInfo(nn::friends::Friend * pOutFriends, const nn::account::Uid & uid, const nn::account::NetworkServiceAccountId * pAccountIds, int count);
nn::Result GetProfileList(nn::friends::AsyncContext * pOutAsync, nn::friends::Profile * pOutProfiles, const nn::account::Uid & uid, const nn::account::NetworkServiceAccountId * pAccountIds, int count);
} }
enum {
    CONTEXT_INVALID = 255,
    REFRESH_INTERVAL = 2000,
    GAME_REFRESH_INTERVAL = 30000,
    FRIENDS_REFRESH_INTERVAL_XBOXONE = 30000,
    FRIENDS_SLOW_REFRESH_INTERVAL_XBOXONE = 600000,
    XBOXONE_RTA_RECONNECT_INTERVAL_MS = 60000,
    PRESENCE_UPDATE_DELAY = 5000,
    PRESENCE_BUSY_DELAY = 20000,
    MAX_PRESENCE_USERS = 4,
    MAX_NAME_CACHE_SIZE = 4000,
    MAX_NAME_CACHE_DURATION = 3600,
};

namespace Console {
namespace Online {

class PresenceManagerInternal;

typedef nn::account::NetworkServiceAccountId PlatformFriend;






























inline OnlineUserId GetPlatformFriendId(const PlatformFriend & tFriend)
{
    return tFriend.id;
}


struct PresenceFriend : public Friend
{
    // (126)
    ~PresenceFriend() {}

    PresenceFriend(PresenceManagerInternal * ptPresenceManager)
        : Friend()
        , m_ptPresenceManager(ptPresenceManager)
        , m_fFlags(0)
        , m_uLastGameInfoUpdate(0)
        , m_bSubscribedTo(FALSE)
    {
        // (139)
        ClearHostInfo();
    }

    void ClearHostInfo()
    {
        // (145-150)
        m_bInGame = FALSE;
        m_tHostAddr.m_addr = NULL;
        SigmaMemoryClear(&m_tHostAddr.m_id, sizeof(m_tHostAddr.m_id));
        SigmaMemoryClear(&m_tHostAddr.m_key, sizeof(m_tHostAddr.m_key));
        m_uLastGameInfoUpdate = 0;
    }

    enum Flag {
        FRIEND_IS_FRIEND = 0,
        FRIEND_IN_PARTY = 1,
        FRIEND_IN_CONTEXT_VIA_PRESENCE = 2,
        FRIEND_IN_CONTEXT_VIA_PARTY = 3,
        FRIEND_FAVORITE = 4,
        FRIEND_DW_ACCOUNT_CHECK_REQUIRED = 5,
        FRIEND_DW_ACCOUNT_CHECK_IN_PROGRESS = 6,
        FRIEND_NAME_QUERY_REQUIRED = 7,
        FRIEND_SUSPECTED_GUEST = 8,
    };

    // (169-186)
    void SetFlag(Flag eFlag, BOOL fValue) { if (fValue) m_fFlags |= (1 << eFlag); else m_fFlags &= ~(1 << eFlag); }
    BOOL TestFlag(Flag eFlag) const { return (m_fFlags >> eFlag) & 1; }

    BOOL IsFriend() const { return TestFlag(FRIEND_IS_FRIEND); }
    BOOL IsInParty() const { return TestFlag(FRIEND_IN_PARTY); }
    BOOL IsValid() const {
        return TestFlag(FRIEND_IS_FRIEND) || TestFlag(FRIEND_IN_PARTY);
    }
    BOOL IsFavorite() const {
        return TestFlag(FRIEND_FAVORITE);
    }
    BOOL IsDWAccountCheckRequired() const {
        return TestFlag(FRIEND_DW_ACCOUNT_CHECK_REQUIRED);
    }
    BOOL HasName() const {
        return m_szUsername[0] != '\0';
    }
    BOOL IsInContext() const {
        return TestFlag(FRIEND_IN_CONTEXT_VIA_PRESENCE) || TestFlag(FRIEND_IN_CONTEXT_VIA_PARTY);
    }

    void ParsePresence(const bdUByte8 * abBuffer, uint32 uDataSize);

    PresenceManagerInternal * m_ptPresenceManager;  // 0xa0
    uint32 m_fFlags;  // 0xa8
    bdQoSRemoteAddr m_tHostAddr;  // 0xb0
    Blizzard::Time::Millisecond m_uLastGameInfoUpdate;  // 0xd0
    BOOL m_bSubscribedTo;  // 0xd4
};
static_assert(sizeof(PresenceFriend) == 0xd8, "PresenceFriend");

















































class PresenceFriendsList : public blz::list<PresenceFriend>
{
public:
    // (249)
    PresenceFriendsList(PresenceManagerInternal * ptPresenceManager)
        : m_ptPresenceManager(ptPresenceManager)
        , m_bFriendsAdded(false)
    {
    }

    PresenceFriend * FindFriend(OnlineUserId idFriend);
    PresenceFriend * AddFriend(const PlatformFriend & tPlatformFriend);
    void RemoveFriendById(OnlineUserId);
    void ClearFlag(PresenceFriend::Flag fValidType);
    void RemoveInvalidFriends();
    // (272-274)
    bool AreFriendsAdded() { return m_bFriendsAdded; }
    void SetFriendsAdded() { m_bFriendsAdded = true; }
    void ClearFriendsAdded() { m_bFriendsAdded = false; }

private:
    PresenceManagerInternal * m_ptPresenceManager;  // 0x18
    bool m_bFriendsAdded;  // 0x20
};
static_assert(sizeof(PresenceFriendsList) == 0x28, "PresenceFriendsList");















class PresenceManagerInternal : public PresenceManager
{
public:
    PresenceManagerInternal(int32 nUserIdx);
    ~PresenceManagerInternal();
    void IdleInternal();
    virtual uint32 GetAllFriends(Friend * ptFriends, uint32 uMaxFriends);
    virtual BOOL HasAnyFriends();
    virtual uint32 GetOnlineFriends(Friend * ptFriends, uint32 uMaxFriends);
    virtual blz::vector<unsigned long> PresenceGetFriendIDs();
    virtual BOOL GetRandomFriend(Friend & tOutFriend, bdOnlineUserID idBlacklistPlayer);
    virtual Friend * GetFriend(OnlineUserId idUser);
    virtual BOOL HasFriendByDemonWareID(bdOnlineUserID idFriend);
    void OnConnect(int32 nUserIndex);
    void OnDisconnect(int32 nUserIndex);
    virtual void OnNX64UserSignedIn();
    virtual void OnNX64UserSignedOut();
    virtual const PartyConnectionInfo & GetConnectionInfo();
    virtual void OnRichPresenceUpdateReceived(const bdRichPresenceData & tPresenceData);
    void OnSubscribedToFriends(bdRichPresenceData * ptResults, bdUInt uNumResults);
    void OnUnsubscribedFromFriends(OnlineUserId * ptidUsers, bdUInt uNumUsers);
    uint32 GetNumberSubscribedToFriends();
    void InvalidateSubscriptions();
    void SubscribeToOnlineFriends();
    void UnsubscribeFromUnwantedFriends();
    virtual void GetProfileNames(const blz::vector<unsigned long> & idUsers, PresenceGetProfileNamesCallback pfnCallback);
    void ProcessNX64FriendsList(const nn::friends::Friend * ptNX64Friends, int32 nNumNX64Friends);
    void UpdateSingleFriend(const nn::friends::Friend & tNX64Friend, BOOL bUpdateSubscription);
    nn::account::Uid GetUserId();
    BOOL RequestPermissionForGetProfileNames();
    void GetFriendsList();
    void ClearFriends();
    void UpdateUser();
    void PerformDWAccountCheck();
    void OnLookupUserAccounts(BOOL bSuccess, OnlineUserId * pidUsers, bdOnlineUserID * pidBDUsers, uint32 uNumUsers);

    int32 m_nUserIndex;  // 0x08
    BOOL m_bIsPrimaryUser;  // 0x0c
    TaskManager m_tTaskManager;  // 0x10
    Blizzard::Lock::RMutex m_tMutex;  // 0x30
    Blizzard::Time::Millisecond m_uLastRefresh;  // 0x50
    BOOL m_bFriendsNeedToBeUpdated;  // 0x54
    int32 m_nFriendsUpdateTasksPending;  // 0x58
    nn::friends::NotificationQueue m_tNotificationQueue;  // 0x60
    BOOL m_bNotificationQueueActive;  // 0xc8
    Blizzard::Time::Millisecond m_uLastPresenceChange;  // 0xcc
    Blizzard::Time::Millisecond m_uNextPresenceUpdateDelay;  // 0xd0
    static blz::list<unsigned int> ms_auNameCheckPermissionsGranted;
    BOOL m_bPresenceDirty;  // 0xd4
    void MarkPresenceDirty();
    static PartyConnectionInfo ms_tConnectionInfo;
    static GamePresence ms_tGamePresence;
    UserPresence m_tUserPresence;  // 0xd8
    BOOL m_bHasPublishedPresenceDetails;  // 0x10c
    PresenceFriendsList m_tFriends;  // 0x110
    static BOOL ms_bListingEnabled;
    static BOOL ms_bConnectionInfoChangedForActivitySession;
};
static_assert(sizeof(PresenceManagerInternal) == 0x138, "PresenceManagerInternal");



































































































































































































inline void PresenceManagerInternal::MarkPresenceDirty()
{
    if (!m_bPresenceDirty)
    {
        m_uLastPresenceChange = Blizzard::Time::Milliseconds();
    }
    m_bPresenceDirty = TRUE;
}



















PartyConnectionInfo PresenceManagerInternal::ms_tConnectionInfo;
GamePresence PresenceManagerInternal::ms_tGamePresence;
BOOL PresenceManagerInternal::ms_bListingEnabled;
BOOL PresenceManagerInternal::ms_bConnectionInfoChangedForActivitySession;



































class PresenceTask : public Task
{
public:
    PresenceTask(PresenceManagerInternal * ptPresenceManager);
    PresenceManagerInternal * m_ptPresenceManager;  // 0x10
};






















enum {
    MAX_USERS_PER_PRESENCE_QUERY = 100,
};

enum {
    MAX_SUBSCRIBED_FRIENDS = 100,
};

class LobbyPresenceTask : public LobbyTask
{
public:
    LobbyPresenceTask(LobbyService * ptLobbyService, PresenceManagerInternal * ptPresenceManager)
        : LobbyTask(ptLobbyService)
        , m_ptPresenceManager(ptPresenceManager)
    {
    }
protected:
    PresenceManagerInternal * m_ptPresenceManager;  // 0x20
};








class UpdatePresenceTask : public LobbyPresenceTask
{
public:
    UpdatePresenceTask(LobbyService * ptLobbyService, PresenceManagerInternal * ptPresenceManager, bdRichPresenceData & tPresenceData)
        : LobbyPresenceTask(ptLobbyService, ptPresenceManager)
        , m_tPresenceData(tPresenceData)
    {
    }

    //@ 669
    virtual void Start()
    {
        m_ptRemoteTask = m_ptLobbyService->GetRichPresence()->setRichPresence(m_tPresenceData);
    }

    //@ 678
    virtual void OnComplete()
    {
    }

    //@ 684
    virtual void OnFailed()
    {
    }

protected:
    bdRichPresenceData m_tPresenceData;  // 0x28
};



















class UnsubscribeFromUsersTask : public LobbyPresenceTask
{
public:
    UnsubscribeFromUsersTask(LobbyService * ptLobbyService, PresenceManagerInternal * ptPresenceManager, const OnlineUserId * ptidUsers, const bdUInt uNumUsers)
        : LobbyPresenceTask(ptLobbyService, ptPresenceManager)
    {
        // (714-719)
        m_uNumUsers = (uNumUsers < MAX_USERS_PER_PRESENCE_QUERY) ? uNumUsers : MAX_USERS_PER_PRESENCE_QUERY;
        for (bdUInt i = 0; i < m_uNumUsers; ++i)
        {
            m_aidUsers[i] = ptidUsers[i];
        }
    }

    //@ 724
    virtual void Start()
    {
        // (727)
        bdUserAccountID artAccounts[MAX_USERS_PER_PRESENCE_QUERY];
        for (bdUInt i = 0; i < m_uNumUsers; ++i)
        {
            // (730)
            artAccounts[i] = bdUserAccountID(m_aidUsers[i], "nintendo");
        }
        // (733)
        m_ptRemoteTask = m_ptLobbyService->GetRichPresence()->unsubscribeFromUsers(artAccounts, m_uNumUsers);
    }
    // NON_MATCHING: OnComplete: x19/x20 swapped (manager vs &m_tMutex) after inlining OnUnsubscribedFromFriends
    //@ 738
    virtual void OnComplete()
    {
        // (743)
        m_ptPresenceManager->OnUnsubscribedFromFriends(m_aidUsers, m_uNumUsers);
    }

    //@ 746
    virtual void OnFailed()
    {
    }

private:
    OnlineUserId m_aidUsers[MAX_USERS_PER_PRESENCE_QUERY];  // 0x28
    bdUInt m_uNumUsers;  // 0x348
};

















class SubscribeToUsersTask : public LobbyPresenceTask
{
public:
    SubscribeToUsersTask(LobbyService * ptLobbyService, PresenceManagerInternal * ptPresenceManager, const OnlineUserId * ptidUsers, const bdUInt uNumUsers)
        : LobbyPresenceTask(ptLobbyService, ptPresenceManager)
    {
        // (775)
        m_uNumUsers = (uNumUsers < MAX_USERS_PER_PRESENCE_QUERY) ? uNumUsers : MAX_USERS_PER_PRESENCE_QUERY;
        // (777)
        if (ptidUsers != NULL)
        {
            for (bdUInt i = 0; i < m_uNumUsers; ++i)
            {
                // (781)
                m_aidUsers[i] = ptidUsers[i];
            }
        }
    }

    //@ 786
    virtual void Start()
    {
        // (789)
        bdUserAccountID artAccounts[MAX_USERS_PER_PRESENCE_QUERY];
        for (bdUInt i = 0; i < m_uNumUsers; ++i)
        {
            // (793)
            artAccounts[i] = bdUserAccountID(m_aidUsers[i], "nintendo");
        }
        // (796)
        m_ptRemoteTask = m_ptLobbyService->GetRichPresence()->getRichPresenceAndSubscribe(artAccounts, m_artPresenceResults, m_uNumUsers);
    }

    //@ 802
    virtual void OnComplete()
    {
        // (806)
        m_ptPresenceManager->OnSubscribedToFriends(m_artPresenceResults, m_uNumUsers);
    }

    //@ 809
    virtual void OnFailed()
    {
    }

private:
    OnlineUserId m_aidUsers[MAX_USERS_PER_PRESENCE_QUERY];  // 0x28
    bdRichPresenceData m_artPresenceResults[MAX_USERS_PER_PRESENCE_QUERY];  // 0x348
    bdUInt m_uNumUsers;  // 0x5ac8
};































PresenceManagerInternal * sptPresenceManager[MAX_PRESENCE_USERS];



blz::list<unsigned int> PresenceManagerInternal::ms_auNameCheckPermissionsGranted;








OnlineService::TimedLRUCache<OnlineUserId, blz::embedded_string<20>, long> stPresenceNameCache(4000, 3600, OnlineServiceServer::GetCurrentUnixTime);








void PresenceManager::Create()
{
    for (int32 i = 0; i < MAX_PRESENCE_USERS; ++i)
    {
        sptPresenceManager[i] = new PresenceManagerInternal(i);
    }
}






PresenceManager * PresenceManager::Get(int32 nUserIdx)
{
    // (884)
    return sptPresenceManager[nUserIdx];
}






PresenceManagerInternal::PresenceManagerInternal(int32 nUserIdx)
    : m_nUserIndex(nUserIdx)
    , m_bIsPrimaryUser(FALSE)
    , m_tTaskManager()
    , m_tMutex()
    , m_uLastRefresh(0)
    , m_bFriendsNeedToBeUpdated(FALSE)
    , m_nFriendsUpdateTasksPending(0)
    , m_tNotificationQueue()
    , m_bNotificationQueueActive(FALSE)
    , m_uLastPresenceChange(0)
    , m_uNextPresenceUpdateDelay(PRESENCE_UPDATE_DELAY)
    , m_bPresenceDirty(FALSE)
    , m_tUserPresence()
    , m_bHasPublishedPresenceDetails(FALSE)
    , m_tFriends(this)
{
    // (918)
    LobbyService::Get(m_nUserIndex)->GetConnectEvent().Subscribe(
        OnlineService::internal::method_binder_0_1<void, PresenceManagerInternal, int32>::bind<&PresenceManagerInternal::OnConnect>(this));
    LobbyService::Get(m_nUserIndex)->GetDisconnectEvent().Subscribe(
        OnlineService::internal::method_binder_0_1<void, PresenceManagerInternal, int32>::bind<&PresenceManagerInternal::OnDisconnect>(this));
}























void PresenceManager::Idle()
{
    if (Console::GamerProfile::GetPrimaryProfileUserIndex() == -1)
    {
        // (942)
        SetListingEnabled(FALSE);
        // (948)
        if (!PresenceManagerInternal::ms_tConnectionInfo.m_ptAddress.isNull())
        {
            PresenceManagerInternal::ms_tConnectionInfo = PartyConnectionInfo();
        }
    }

    // (960)
    for (int32 i = 0; i < MAX_PRESENCE_USERS; ++i)
    {
        sptPresenceManager[i]->IdleInternal();
    }
}


















































































void PresenceManagerInternal::IdleInternal()
{
    // (1042)
    m_bIsPrimaryUser = (m_nUserIndex == Console::GamerProfile::GetPrimaryProfileUserIndex());
    BOOL bSignedInOnline = Console::GamerProfile::IsSignedInOnline(m_nUserIndex);

    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();














































































































































    if (!bSignedInOnline && m_nFriendsUpdateTasksPending == 0)
    {
        ClearFriends();
    }







    if (m_bFriendsNeedToBeUpdated && m_nFriendsUpdateTasksPending == 0)
    {
        m_bFriendsNeedToBeUpdated = FALSE;

        if (bSignedInOnline)
        {
            GetFriendsList();
        }
    }

    Blizzard::Time::Millisecond uTimeSinceLastRefresh = Blizzard::Time::GetTimeElapsed(m_uLastRefresh, now);
    if (uTimeSinceLastRefresh > REFRESH_INTERVAL)
    {
        UpdateUser();
        m_uLastRefresh = now;
    }



























































    nn::friends::NotificationInfo tNotificationInfo = {};
    {
        Console::GamerProfile::ScopedUsersReadLock tScopedLock;
        while (m_bNotificationQueueActive && m_tNotificationQueue.Pop(&tNotificationInfo).IsSuccess())
        {
            if (tNotificationInfo.type == nn::friends::NotificationType_FriendListUpdated)
            {
                // (1283)
                m_bFriendsNeedToBeUpdated = TRUE;
                break;
            }
            else if (tNotificationInfo.type == nn::friends::NotificationType_FriendPresenceUpdated)
            {
                // (1289)
                nn::account::NetworkServiceAccountId idFriendAccount = tNotificationInfo.accountId;
                nn::friends::Friend tUpdatedFriend;
                nn::Result tResult = nn::friends::UpdateFriendInfo(&tUpdatedFriend, GetUserId(), &idFriendAccount, 1);
                if (tResult.IsSuccess() && tUpdatedFriend.IsValid())
                {
                    UpdateSingleFriend(tUpdatedFriend, TRUE);
                }
            }
        }
    }


    if (m_tFriends.AreFriendsAdded())
    {
        m_tFriends.ClearFriendsAdded();

        PerformDWAccountCheck();
    }













    TaskManager::ProcessMode eProcessMode = bSignedInOnline ? TaskManager::PROCESS_CAN_START_TASKS : TaskManager::PROCESS_FAIL_NOT_STARTED_TASKS;




    m_tTaskManager.Process(eProcessMode);
}




void PresenceManager::SetListingEnabled(BOOL bEnabled)
{
    // (1334)
    if (PresenceManagerInternal::ms_bListingEnabled != bEnabled)
    {
        PresenceManagerInternal::ms_bListingEnabled = bEnabled;
    }
}







void PresenceManager::SetSessionAndGamePresence(const PartyConnectionInfo & tConnectionInfo, const GamePresence & tGamePresence)
{
    // (1349)
    BOOL bChangeApplied = FALSE;

    // (1352)
    if (!IsSameAddress(PresenceManagerInternal::ms_tConnectionInfo.m_ptAddress, tConnectionInfo.m_ptAddress) ||
        memcmp(&PresenceManagerInternal::ms_tConnectionInfo.m_tSecID, &tConnectionInfo.m_tSecID, sizeof(bdSecurityID)) != 0 ||
        memcmp(&PresenceManagerInternal::ms_tConnectionInfo.m_tSecKey, &tConnectionInfo.m_tSecKey, sizeof(bdSecurityKey)) != 0)
    {
        PresenceManagerInternal::ms_tConnectionInfo = tConnectionInfo;
        PresenceManagerInternal::ms_bConnectionInfoChangedForActivitySession = TRUE;
        bChangeApplied = TRUE;
    }

    // (1361)
    if (memcmp(&PresenceManagerInternal::ms_tGamePresence, &tGamePresence, sizeof(GamePresence)) != 0)
    {
        PresenceManagerInternal::ms_tGamePresence = tGamePresence;
        bChangeApplied = TRUE;
    }

    // (1367)
    if (bChangeApplied)
    {
        for (int32 i = 0; i < MAX_PRESENCE_USERS; ++i)
        {
            sptPresenceManager[i]->MarkPresenceDirty();
        }
    }
}














void PresenceGetProfileNames(int32 nUserIndex, const blz::vector<OnlineUserId> & idUsers, PresenceGetProfileNamesCallback pfnCallback)
{
    // (1396)
    PresenceManager::Get(nUserIndex)->GetProfileNames(idUsers, pfnCallback);
}













































































































































































































































































struct GetProfileNamesBatchHandler
{
    blz::vector<OnlineUserId> idUsers;  // 0x00
    PresenceProfileNameMap tResultsMap;  // 0x18
    PresenceGetProfileNamesCallback pfnCallback;  // 0x40
};

typedef OnlineService::Callback<void (BOOL)> GetProfileNamesTaskCallback;





















































































































class GetProfileNamesBatchTask : public PresenceTask
{
public:
    GetProfileNamesBatchTask(PresenceManagerInternal * ptPresenceManager, blz::shared_ptr<GetProfileNamesBatchHandler> ptBatchHandler, GetProfileNamesTaskCallback pfnCallback);
    virtual ~GetProfileNamesBatchTask();
    virtual void Start();
    virtual void OnCancel();
    virtual void OnComplete();
    virtual void OnFailed();
    virtual Task::Status GetStatus();
private:
    BOOL m_bStarted;  // 0x18
    nn::friends::AsyncContext m_tAsyncContext;  // 0x20
    nn::friends::Profile m_artProfileResults[100];  // 0x48
    int32 m_nNumRequested;  // 0x6448
    blz::shared_ptr<GetProfileNamesBatchHandler> m_ptBatchHandler;  // 0x6450
    GetProfileNamesTaskCallback m_pfnCallback;  // 0x6460
    nn::account::Uid m_nidOwner;  // 0x6468
};
GetProfileNamesBatchTask::~GetProfileNamesBatchTask() {}


GetProfileNamesBatchTask::GetProfileNamesBatchTask(PresenceManagerInternal * ptPresenceManager, blz::shared_ptr<GetProfileNamesBatchHandler> ptBatchHandler, GetProfileNamesTaskCallback pfnCallback)
    // (1816)
    : PresenceTask(ptPresenceManager)
    , m_bStarted(FALSE)
    , m_tAsyncContext()
    , m_nNumRequested(0)
    , m_ptBatchHandler(ptBatchHandler)
    , m_pfnCallback(pfnCallback)
    , m_nidOwner(ptPresenceManager->GetUserId())
{
}





void GetProfileNamesBatchTask::Start()
{
    // (1831)
    if (IsPhysicalNetworkConnected())
    {
        nn::account::NetworkServiceAccountId aridNSA[MAX_USERS_PER_PRESENCE_QUERY];
        m_nNumRequested = m_ptBatchHandler->idUsers.size() < MAX_USERS_PER_PRESENCE_QUERY ?
            m_ptBatchHandler->idUsers.size() : MAX_USERS_PER_PRESENCE_QUERY;

        // (1837)
        for (size_t i = 0; i < m_nNumRequested; ++i)
        {
            aridNSA[i].id = m_ptBatchHandler->idUsers[i];
        }
        m_ptBatchHandler->idUsers.erase(m_ptBatchHandler->idUsers.begin(), m_ptBatchHandler->idUsers.begin() + m_nNumRequested);

        // (1843)
        nn::friends::GetProfileList(&m_tAsyncContext, m_artProfileResults, m_nidOwner, aridNSA, m_nNumRequested);

        m_bStarted = TRUE;
    }
}

void GetProfileNamesBatchTask::OnCancel()
{
    // (1856)
    m_tAsyncContext.Cancel();
}




void GetProfileNamesBatchTask::OnComplete()
{
    for (int32 i = 0; i < m_nNumRequested; ++i)
    {
        nn::friends::Profile & tProfile = m_artProfileResults[i];
        if (tProfile.IsValid())
        {
            char szName[32];
            Blizzard::String::Copy(szName, tProfile.GetNickname().name, sizeof(szName));
            OnlineUserId idUser = tProfile.GetAccountId().id;

            m_ptBatchHandler->tResultsMap[idUser] = szName;
            *stPresenceNameCache[idUser] = szName;
        }
    }

    // (1878)
    m_pfnCallback(TRUE);
}


void GetProfileNamesBatchTask::OnFailed()
{
    m_pfnCallback(FALSE);
}



Task::Status GetProfileNamesBatchTask::GetStatus()
{
    Task::Status eStatus;
    // (1892)
    if (!m_bStarted)
    {
        eStatus = TASK_NOT_STARTED;
    }
    else
    {
        bool bDone = false;
        // (1899)
        if (m_tAsyncContext.HasDone(&bDone).IsFailure())
        {
            eStatus = TASK_FAILED;
        }
        // (1902)
        else if (bDone)
        {
            nn::Result tTaskResult = m_tAsyncContext.GetResult();
            // (1908)
            eStatus = tTaskResult.IsSuccess() ? TASK_DONE : TASK_FAILED;
        }
        else
        {
            eStatus = TASK_IN_PROGRESS;
        }
    }
    // (1923)
    return eStatus;
}


























}}  namespace { using namespace Console::Online;  // NON_MATCHING sOnGetProfileNamesBatch: 704 vs 688; the original reloads the GetProfileNamesTaskCallback temporary from the stack and null-checks it after the task ctor, and keeps bSuccess as a live argument (ours drops it, so call_callback is 180 vs 192)
void sOnGetProfileNamesBatch(PresenceManagerInternal * ptPresenceManager, blz::shared_ptr<GetProfileNamesBatchHandler> ptBatchHandler, BOOL bSuccess)
{
    // (1951)
    if (!ptBatchHandler->idUsers.empty())
    {
        // (1954)
        BOOL bCanContinue = ptPresenceManager != NULL && ptPresenceManager->RequestPermissionForGetProfileNames();
        if (!bCanContinue)
        {
            // (1957)
            ptBatchHandler->pfnCallback(!ptBatchHandler->tResultsMap.empty(), ptBatchHandler->tResultsMap);
        }
        else
        {
            // (1962)
            ::new GetProfileNamesBatchTask(ptPresenceManager, ptBatchHandler, GetProfileNamesTaskCallback(OnlineService::internal::function_binder_3<void, PresenceManagerInternal *, blz::shared_ptr<GetProfileNamesBatchHandler>, BOOL>::bind<&sOnGetProfileNamesBatch>(ptPresenceManager, ptBatchHandler)));
        }
    }
    else
    {
        ptBatchHandler->pfnCallback(!ptBatchHandler->tResultsMap.empty(), ptBatchHandler->tResultsMap);
    }
}
}  namespace Console { namespace Online {
void PresenceManagerInternal::GetProfileNames(const blz::vector<OnlineUserId> & idUsers, PresenceGetProfileNamesCallback pfnCallback)  //NON_MATCHING: 1728 vs 1732; only difference: the original passes TRUE in w2 to sOnGetProfileNamesBatch (ours drops the unused argument via dead-argument elimination)
{
    // (1974)
    blz::shared_ptr<GetProfileNamesBatchHandler> ptBatchHandler = blz::make_shared<GetProfileNamesBatchHandler>();

    ptBatchHandler->idUsers.reserve(idUsers.size());
    ptBatchHandler->pfnCallback = pfnCallback;

    for (OnlineUserId idUser : idUsers)
    {
        if (idUser != 0)
        {
            blz::embedded_string<20> * ptCachedName = stPresenceNameCache.get(idUser);
            if (ptCachedName)
            {
                ptBatchHandler->tResultsMap[idUser] = *ptCachedName;
            }
            else
            {
                ptBatchHandler->idUsers.push_back(idUser);
            }
        }
    }

    // (1997)
    sOnGetProfileNamesBatch(this, ptBatchHandler, TRUE);
}




















































































































































































































































































void PresenceManagerInternal::ProcessNX64FriendsList(const nn::friends::Friend * ptNX64Friends, int32 nNumNX64Friends)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (2280)
    m_tFriends.ClearFlag(PresenceFriend::FRIEND_IS_FRIEND);

    if (ptNX64Friends)
    {
        // (2288)
        for (int32 i = 0; i < nNumNX64Friends; ++i)
        {
            const nn::friends::Friend & tFriendEntry = ptNX64Friends[i];
            UpdateSingleFriend(tFriendEntry, FALSE);
        }

        // (2295)
        UnsubscribeFromUnwantedFriends();
        m_tFriends.RemoveInvalidFriends();

        SubscribeToOnlineFriends();
    }
}





void PresenceManagerInternal::UpdateSingleFriend(const nn::friends::Friend & tNX64Friend, BOOL bUpdateSubscription)
{
    // (2305)
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    const PlatformFriend & tPlatFriend = tNX64Friend.GetAccountId();
    const OnlineUserId idFriend = GetPlatformFriendId(tPlatFriend);

    // (2311)
    PresenceFriend * ptPresenceFriend = m_tFriends.FindFriend(idFriend);
    if (!ptPresenceFriend)
    {
        ptPresenceFriend = m_tFriends.AddFriend(tPlatFriend);
    }

    {
        // (2320)
        ptPresenceFriend->SetFlag(PresenceFriend::FRIEND_IS_FRIEND, TRUE);

        const BOOL bFavorite = tNX64Friend.IsFavorite();
        ptPresenceFriend->SetFlag(PresenceFriend::FRIEND_FAVORITE, bFavorite);

        // (2327)
        nn::friends::PresenceStatus ePresenceStatus = tNX64Friend.GetPresence().GetStatus();
        BOOL bOnline = ePresenceStatus > nn::friends::PresenceStatus_Offline;

        BOOL bSameAppGroup = tNX64Friend.GetPresence().IsSamePresenceGroupApplication();
        BOOL bInContext = bOnline && bSameAppGroup;
        ptPresenceFriend->SetFlag(PresenceFriend::FRIEND_IN_CONTEXT_VIA_PRESENCE, bInContext);

        // (2336)
        if (bUpdateSubscription && LobbyService::Get(m_nUserIndex)->IsConnected())
        {
            if (bInContext)
            {
                if (!ptPresenceFriend->m_bSubscribedTo && GetNumberSubscribedToFriends() < 100)
                {
                    ::new SubscribeToUsersTask(LobbyService::Get(m_nUserIndex), this, &idFriend, 1);
                }
            }
            else if (ptPresenceFriend->m_bSubscribedTo)
            {
                ::new UnsubscribeFromUsersTask(LobbyService::Get(m_nUserIndex), this, &idFriend, 1);
            }
        }
    }

    // (2354)
    Blizzard::String::Copy(ptPresenceFriend->m_szUsername, tNX64Friend.GetNickname().name, sizeof(ptPresenceFriend->m_szUsername));

    *stPresenceNameCache[idFriend] = tNX64Friend.GetNickname().name;
}










nn::account::Uid PresenceManagerInternal::GetUserId()
{
    return Console::GamerProfile::GetUserId(m_nUserIndex);
}

BOOL PresenceManagerInternal::RequestPermissionForGetProfileNames()
{
    // (2373)
    Blizzard::Time::Second uNow = Blizzard::Time::Seconds();
    Blizzard::Time::Second uRollingBoundary = uNow - 60;
    // (2377)
    ms_auNameCheckPermissionsGranted.remove_if(
        [uRollingBoundary](uint32 uGrantTime) { return uGrantTime < uRollingBoundary; });
    // (2381)
    uint32 uGrantsInRollingPeriod = ms_auNameCheckPermissionsGranted.size();
    if (uGrantsInRollingPeriod < 10)
    {
        // (2384)
        ms_auNameCheckPermissionsGranted.push_back(uNow);
        return TRUE;
    }
    return FALSE;
}














































































































































































void PresenceFriend::ParsePresence(const bdUByte8 * abBuffer, uint32 uDataSize)
{
    if (uDataSize > 0)
    {
        // (2575)
        bdBitBufferRef ptBitBuffer = ::new bdBitBuffer(abBuffer, uDataSize * 8, true);
        uint32 uVersion;
        bdBool bOk = ptBitBuffer->readUInt32(uVersion) && uVersion == 0x115;
        PartyConnectionInfo tConnectionInfo;

        // (2583)
        bOk = bOk && tConnectionInfo.Deserialize(ptBitBuffer);
        bOk = bOk && m_tGamePresence.Deserialize(*ptBitBuffer);
        bOk = bOk && m_tUserPresence.Deserialize(*ptBitBuffer);

        if (bOk)
        {
            m_bInGame = TRUE;
            m_tHostAddr.m_addr = tConnectionInfo.m_ptAddress;
            m_tHostAddr.m_id = tConnectionInfo.m_tSecID;
            m_tHostAddr.m_key = tConnectionInfo.m_tSecKey;
        }
        else
        {
            // (2596)
            ClearHostInfo();
        }
    }
    else
    {
        // (2601)
        ClearHostInfo();
    }
}





























































































































































































PresenceFriend * PresenceFriendsList::FindFriend(OnlineUserId idFriend)
{
    for (PresenceFriend & tPresenceFriend : *this)
    {
        if (tPresenceFriend.m_idFriend == idFriend)
        {
            return &tPresenceFriend;
        }
    }
    return NULL;
}







PresenceFriend * PresenceFriendsList::AddFriend(const PlatformFriend & tPlatformFriend)
{
    emplace_back(m_ptPresenceManager);
    PresenceFriend * ptFriend = &back();
    ptFriend->m_idFriend = GetPlatformFriendId(tPlatformFriend);
    // (2812)
    ptFriend->SetFlag(PresenceFriend::FRIEND_DW_ACCOUNT_CHECK_REQUIRED, TRUE);
    SetFriendsAdded();
    return ptFriend;
}






























void PresenceFriendsList::ClearFlag(PresenceFriend::Flag fValidType)
{
    // (2844)
    for (PresenceFriend & tFriend : *this)
    {
        tFriend.SetFlag(fValidType, FALSE);
    }
}




void PresenceFriendsList::RemoveInvalidFriends()
{
    // (2856)
    remove_if([](const PresenceFriend & tFriend) {
        return !tFriend.IsValid();
    });
}





























































































































































void PresenceManagerInternal::GetFriendsList()
{
    static nn::friends::Friend arCachedFriends[300];

    int32 nCount;
    nn::friends::FriendFilter tFilter = {};
    // (3030)
    nn::account::Uid idUser = GetUserId();
    nn::Result tResult = nn::friends::GetFriendList(&nCount, arCachedFriends, idUser, 0, 300, tFilter);

    if (tResult.IsSuccess())
    {
        ProcessNX64FriendsList(arCachedFriends, nCount);
    }
}



























void PresenceManagerInternal::ClearFriends()
{
    // (3062)
    if (!m_tFriends.empty())
    {























        m_tFriends.clear();
    }
}






























































































































void PresenceManagerInternal::UpdateUser()
{
    BOOL bSetPresence = FALSE;
    bdBitBufferRef ptPresenceBitBuffer;

    // (3251)
    if (!Console::GamerProfile::IsSignedIn(m_nUserIndex) ||
        !Console::GamerProfile::IsHeroLoaded(m_nUserIndex))
    {
        // (3254)
        if (m_tUserPresence.m_eClass != -1)
        {
            m_tUserPresence.m_eClass = -1;
            MarkPresenceDirty();
        }
    }

    // (3262)
    if (Console::GamerProfile::IsHeroLoaded(m_nUserIndex))
    {
        const D3::Hero::SavedDefinition & tHeroDefinition = Console::GamerProfile::GetHero(m_nUserIndex);

        // (3266)
        bool bFemale = (tHeroDefinition.digest().player_flags() & 2) != 0;
        PlayerClass ePlayerClass = PlayerClassFromGBID(tHeroDefinition.digest().gbid_class());
        if (ePlayerClass != m_tUserPresence.m_eClass || bFemale != m_tUserPresence.m_bFemale)
        {
            m_tUserPresence.m_eClass = ePlayerClass;
            m_tUserPresence.m_bFemale = bFemale;
            MarkPresenceDirty();
        }

        // (3275)
        uint32 uHeroLevel = Math::Pin((uint32)tHeroDefinition.digest().level(), 1u, (uint32)HERO_LEVEL_MAX);
        if (uHeroLevel != m_tUserPresence.m_uLevel)
        {
            m_tUserPresence.m_uLevel = uHeroLevel;
            MarkPresenceDirty();
        }

        // (3282)
        const D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(m_nUserIndex);
        D3AccountPartition idPartition = GetPartition(tHeroDefinition.digest().player_flags() & 1,
            tHeroDefinition.digest().season_created() >= tAccountDefinition.digest().season_id());

        // (3290)
        uint32 uParagonLevel = Math::Pin(tAccountDefinition.partitions(idPartition).alt_level(), 0u, (uint32)HERO_ALT_LEVEL_MAX);
        if (uParagonLevel != m_tUserPresence.m_uParagonLevel)
        {
            m_tUserPresence.m_uParagonLevel = uParagonLevel;
            MarkPresenceDirty();
        }

        // (3297)
        VisualEquipment tVisualEquipment;
        tVisualEquipment.Import(tHeroDefinition.digest().visual_equipment());
        GBID gbidPortrait = tVisualEquipment.tCosmeticItems[3].gbidItem;
        if (gbidPortrait != m_tUserPresence.m_gbidPortrait)
        {
            m_tUserPresence.m_gbidPortrait = gbidPortrait;
            MarkPresenceDirty();
        }
    }

    // (3366)
    if ((Console::GamerProfile::IsSignedInOnline(m_nUserIndex) || m_bHasPublishedPresenceDetails) &&
        Blizzard::Time::GetTimeElapsed(m_uLastPresenceChange, Blizzard::Time::Milliseconds()) >= m_uNextPresenceUpdateDelay)
    {
        // (3376)
        if (!ms_tConnectionInfo.m_ptAddress.isNull() &&
            m_tUserPresence.m_eClass != -1 &&
            PartyIsInParty(m_nUserIndex))
        {
            // (3389)
            if (m_bPresenceDirty || !m_bHasPublishedPresenceDetails)
            {
                bSetPresence = TRUE;
                m_bHasPublishedPresenceDetails = TRUE;

                // (3397)
                ptPresenceBitBuffer = ::new bdBitBuffer(0, false);
                ptPresenceBitBuffer->writeUInt32(0x115);
                ms_tConnectionInfo.Serialize(ptPresenceBitBuffer);
                ms_tGamePresence.Serialize(*ptPresenceBitBuffer);
                m_tUserPresence.Serialize(*ptPresenceBitBuffer);
            }
        }
        // (3381)
        else if (m_bHasPublishedPresenceDetails)
        {
            bSetPresence = TRUE;
            m_bHasPublishedPresenceDetails = FALSE;
        }

        // (3422)
        if (bSetPresence && Console::GamerProfile::IsSignedIn(m_nUserIndex) &&
            LobbyService::Get(m_nUserIndex)->IsConnected())
        {
            // (3425)
            if (!ptPresenceBitBuffer.isNull())
            {
                bdRichPresenceData tRichPresence(BD_RICH_PRESENCE_STATUS_ONLINE,
                    ptPresenceBitBuffer->getData(), ptPresenceBitBuffer->getDataSize());
                // (3432)
                ::new UpdatePresenceTask(LobbyService::Get(m_nUserIndex), this, tRichPresence);
                m_uLastPresenceChange = Blizzard::Time::Milliseconds();
            }
        }

        // (3467)
        m_bPresenceDirty = FALSE;
    }
}
















































































































































void PresenceManagerInternal::PerformDWAccountCheck()
{
    if (LobbyService::Get(m_nUserIndex)->IsConnected())
    {
        OnlineUserId idUsers[50];
        uint32 uNumUsers = 0;

        for (PresenceFriend & tPresenceFriend : m_tFriends)
        {
            // (3487)
            if (tPresenceFriend.IsDWAccountCheckRequired())
            {
                tPresenceFriend.SetFlag(PresenceFriend::FRIEND_DW_ACCOUNT_CHECK_REQUIRED, FALSE);
                tPresenceFriend.SetFlag(PresenceFriend::FRIEND_DW_ACCOUNT_CHECK_IN_PROGRESS, TRUE);
                idUsers[uNumUsers++] = tPresenceFriend.m_idFriend;

                if (uNumUsers == 50)
                {
                    // (3496)
                    LobbyService::Get(m_nUserIndex)->LookupUserAccounts(idUsers, uNumUsers,
                        OnlineService::internal::method_binder_0_4<void, PresenceManagerInternal, BOOL, OnlineUserId *, bdOnlineUserID *, uint32>::bind<&PresenceManagerInternal::OnLookupUserAccounts>(this));
                    uNumUsers = 0;
                }
            }
        }

        if (uNumUsers > 0)
        {
            // (3504)
            LobbyService::Get(m_nUserIndex)->LookupUserAccounts(idUsers, uNumUsers,
                OnlineService::internal::method_binder_0_4<void, PresenceManagerInternal, BOOL, OnlineUserId *, bdOnlineUserID *, uint32>::bind<&PresenceManagerInternal::OnLookupUserAccounts>(this));
        }
    }
}




void PresenceManagerInternal::OnLookupUserAccounts(BOOL bSuccess, OnlineUserId * pidUsers, bdOnlineUserID * pidBDUsers, uint32 uNumUsers)
{
    for (uint32 i = 0; i < uNumUsers; ++i)
    {
        OnlineUserId idUser = pidUsers[i];
        // (3521)
        PresenceFriend * ptFriend = m_tFriends.FindFriend(idUser);

        if (ptFriend && ptFriend->IsFriend())
        {
            // (3530)
            ptFriend->SetFlag(PresenceFriend::FRIEND_DW_ACCOUNT_CHECK_IN_PROGRESS, FALSE);

            if (bSuccess)
            {
                // (3534)
                ptFriend->m_idBDUser = pidBDUsers[i];
            }
            else if (!LobbyService::Get(m_nUserIndex)->IsConnected())
            {
                // (3539)
                ptFriend->SetFlag(PresenceFriend::FRIEND_DW_ACCOUNT_CHECK_REQUIRED, TRUE);
            }
        }
    }
}










PresenceTask::PresenceTask(PresenceManagerInternal * ptPresenceManager)
    : Task(ptPresenceManager->m_tTaskManager)
    , m_ptPresenceManager(ptPresenceManager)
{
}





uint32 PresenceManagerInternal::GetAllFriends(Friend * ptFriends, uint32 uMaxFriends)
{
    uint32 uNumFriends = 0;
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        if (tPresenceFriend.IsFriend() && tPresenceFriend.HasName() && tPresenceFriend.HasDWAccount())
        {
            // (3573)
            Friend & tFriend = ptFriends[uNumFriends++];
            tFriend = tPresenceFriend;
            tFriend.m_bInPlatformParty = tPresenceFriend.IsInParty();

            if (!tFriend.m_bInGame)
            {
                // (3579)
                tFriend.m_tUserPresence.m_eClass = -1;
                tFriend.m_tUserPresence.m_uLevel = 0;
            }

            if (uNumFriends >= uMaxFriends)
            {
                break;
            }
        }
    }

    return uNumFriends;
}








BOOL PresenceManagerInternal::HasAnyFriends()
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        if (tPresenceFriend.IsFriend() && tPresenceFriend.HasDWAccount())
        {
            return TRUE;
        }
    }

    return FALSE;
}






uint32 PresenceManagerInternal::GetOnlineFriends(Friend * ptFriends, uint32 uMaxFriends)
{
    uint32 uNumFriends = 0;
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (3645)
    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        BOOL bSkip = FALSE;
        if (ms_tConnectionInfo.m_ptAddress.notNull())
        {
            // (3648)
            bSkip = IsSameAddress(ms_tConnectionInfo.m_ptAddress, tPresenceFriend.m_tHostAddr.m_addr);
        }

        if (bSkip)
        {
            continue;
        }

        // (3655)
        if (tPresenceFriend.TestFlag(PresenceFriend::FRIEND_SUSPECTED_GUEST))
        {
            continue;
        }

        // (3665)
        if (tPresenceFriend.IsInContext() && tPresenceFriend.HasName() && tPresenceFriend.m_bSubscribedTo && tPresenceFriend.m_bInGame)
        {
            Friend & tFriend = ptFriends[uNumFriends++];
            tFriend = tPresenceFriend;
            // (3691)
            tFriend.m_bInPlatformParty = tPresenceFriend.IsInParty();
            tFriend.m_bFavorite = tPresenceFriend.IsFavorite();

            if (!tFriend.m_bInGame)
            {
                tFriend.m_tUserPresence.m_eClass = -1;
                tFriend.m_tUserPresence.m_uLevel = 0;
            }

            if (uNumFriends >= uMaxFriends)
            {
                break;
            }
        }
    }

    return uNumFriends;
}



















































blz::vector<unsigned long> PresenceManagerInternal::PresenceGetFriendIDs()
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (3721)
    blz::vector<unsigned long> tResults;
    tResults.reserve(m_tFriends.size());

    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        if (tPresenceFriend.IsFriend() && tPresenceFriend.HasDWAccount())
        {
            // (3729)
            tResults.push_back(tPresenceFriend.m_idBDUser);
        }
    }

    return tResults;
}



// NON_MATCHING: 480 vs 468; laid out on the DWARF/line-table rows (split signature, early return at 3759, locals at 3764/3767, ++ at 3776) and all owner-file rows now match; remaining diff is inside the inlined vector::push_back/_grow (csinc vs csel, an extra branch and register coalescing in the grow path; header vector.h rows 574-575 differ from ours), tried empty()/size()==0 and both && orders
BOOL PresenceManagerInternal::GetRandomFriend(
    Friend & tOutFriend,
    bdOnlineUserID idBlacklistPlayer)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);


    blz::vector<Friend *> aptEligibleFriends;
    aptEligibleFriends.reserve(128);

    for (PresenceFriend & tFriend : m_tFriends)
    {
        if (tFriend.IsFriend() &&
            tFriend.HasName() &&
            tFriend.HasDWAccount())
        {
            aptEligibleFriends.push_back(&tFriend);
        }
    }

    if (aptEligibleFriends.size() == 0)
    {
        return FALSE;
    }

    RNG tRNG;
    RandInit(tRNG);

    uint32 uRandFriendIndex = Rand_Internal(tRNG, aptEligibleFriends.size() - 1);

    tOutFriend = *aptEligibleFriends[uRandFriendIndex];
    // && operand order from the line table (3771: member load col 19, && col 51)
    if (tOutFriend.m_idBDUser == idBlacklistPlayer && aptEligibleFriends.size() > 1)
    {



        ++uRandFriendIndex;
        if (uRandFriendIndex >= aptEligibleFriends.size())
        {
            uRandFriendIndex = 0;
        }

        tOutFriend = *aptEligibleFriends[uRandFriendIndex];
    }

    return TRUE;
}




Friend * PresenceManagerInternal::GetFriend(OnlineUserId idUser)
{
    // (3794)
    return m_tFriends.FindFriend(idUser);
}




BOOL PresenceManagerInternal::HasFriendByDemonWareID(bdOnlineUserID idFriend)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    for (const PresenceFriend & tPresenceFriend : m_tFriends)
    {
        if (tPresenceFriend.IsFriend() && tPresenceFriend.HasName() && tPresenceFriend.HasDWAccount() && tPresenceFriend.m_idBDUser == idFriend)
        {
            return TRUE;
        }
    }

    return FALSE;
}








void PresenceManagerInternal::OnConnect(int32 nUserIndex)
{
    // (3825)
    PerformDWAccountCheck();

    if (m_nUserIndex == nUserIndex)
    {
        // (3830)
        SubscribeToOnlineFriends();
    }
}







void PresenceManagerInternal::OnDisconnect(int32 nUserIndex)
{
    if (m_nUserIndex == nUserIndex)
    {
        InvalidateSubscriptions();
    }
}





void PresenceManagerInternal::OnNX64UserSignedIn()
{
    Console::GamerProfile::ScopedUsersWriteLock tScopedLock;

    m_bFriendsNeedToBeUpdated = TRUE;

    // (3862)
    nn::account::Uid idUser;
    nn::account::UserHandle tUserHandle = Console::GamerProfile::GetUserHandle(m_nUserIndex);
    nn::account::GetUserId(&idUser, tUserHandle);

    m_tNotificationQueue.Initialize(idUser);
    m_bNotificationQueueActive = TRUE;
}






void PresenceManagerInternal::OnNX64UserSignedOut()
{
    Console::GamerProfile::ScopedUsersWriteLock tScopedLock;

    // (3878)
    m_tNotificationQueue.Finalize();
    m_bNotificationQueueActive = FALSE;
}





const PartyConnectionInfo & PresenceManagerInternal::GetConnectionInfo()
{
    return ms_tConnectionInfo;
}




void PresenceManagerInternal::OnRichPresenceUpdateReceived(const bdRichPresenceData & tPresenceData)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    PresenceFriend * ptFriend = m_tFriends.FindFriend(tPresenceData.getUserAccountID().m_userID);
    if (ptFriend)
    {
        // (3901)
        ptFriend->ParsePresence(tPresenceData.getPresenceData(), tPresenceData.getPresenceDataSize());
    }
}




void PresenceManagerInternal::OnSubscribedToFriends(bdRichPresenceData * ptResults, bdUInt uNumResults)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    if (ptResults)
    {
        for (bdUInt i = 0; i < uNumResults; ++i)
        {
            OnRichPresenceUpdateReceived(ptResults[i]);

            OnlineUserId idFriend = ptResults[i].getUserAccountID().m_userID;
            PresenceFriend * ptFriend = m_tFriends.FindFriend(idFriend);
            if (!ptFriend)
            {
                PlatformFriend idNSA;
                idNSA.id = idFriend;
                ptFriend = m_tFriends.AddFriend(idNSA);
            }

            ptFriend->m_bSubscribedTo = TRUE;
        }

        UnsubscribeFromUnwantedFriends();
        // (3941)
        m_tFriends.RemoveInvalidFriends();
    }
}









// not inline: the DWARF has a concrete out-of-line copy (low_pc 0, linker-removed) and no declared-inline flag
void PresenceManagerInternal::OnUnsubscribedFromFriends(
    OnlineUserId * ptidUsers,
    bdUInt uNumUsers)
{
    Blizzard::Lock::ScopedMutex tLock(m_tMutex);

    if (uNumUsers > 0)
    {
        for (bdUInt i = 0; i < uNumUsers; ++i)
        {
            PresenceFriend * ptFriend = m_tFriends.FindFriend(ptidUsers[i]);

            if (ptFriend)
            {

                ptFriend->m_bSubscribedTo = FALSE;
            }
        }





        SubscribeToOnlineFriends();
    }
}

uint32 PresenceManagerInternal::GetNumberSubscribedToFriends()
{
    uint32 uNumSubscribedTo = 0;
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (3978)
    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        if (tPresenceFriend.m_bSubscribedTo)
        {
            ++uNumSubscribedTo;
        }
    }

    return uNumSubscribedTo;
}

void PresenceManagerInternal::InvalidateSubscriptions()
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (3994)
    for (PresenceFriend & tPresenceFriend : m_tFriends)
    {
        tPresenceFriend.m_bSubscribedTo = FALSE;
    }
}

void PresenceManagerInternal::SubscribeToOnlineFriends()
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (4005)
    if (LobbyService::Get(m_nUserIndex)->IsConnected())
    {
        OnlineUserId aidUsers[MAX_USERS_PER_PRESENCE_QUERY];

        // (4011)
        uint32 uNumSubscribed = GetNumberSubscribedToFriends();
        if (uNumSubscribed < MAX_SUBSCRIBED_FRIENDS)
        {
            uint32 uNumToSubscribe = 0;

            // (4022)
            for (PresenceFriend & tPresenceFriend : m_tFriends)
            {
                if (uNumToSubscribe >= MAX_USERS_PER_PRESENCE_QUERY)
                {
                    break;
                }

                // (4031)
                if (uNumToSubscribe + uNumSubscribed >= MAX_SUBSCRIBED_FRIENDS)
                {
                    break;
                }

                // (4037)
                if (tPresenceFriend.IsInContext() && !tPresenceFriend.m_bSubscribedTo)
                {
                    // (4047)
                    aidUsers[uNumToSubscribe] = tPresenceFriend.m_idFriend;
                    uNumToSubscribe++;
                }
            }

            // (4052)
            if (uNumToSubscribe > 0)
            {
                ::new SubscribeToUsersTask(LobbyService::Get(m_nUserIndex), this, aidUsers, uNumToSubscribe);
            }
        }
    }
}













void PresenceManagerInternal::UnsubscribeFromUnwantedFriends()
{
    Blizzard::Lock::ScopedRMutex tLock(m_tMutex);

    // (4063)
    if (!LobbyService::Get(m_nUserIndex)->IsConnected())
    {
        InvalidateSubscriptions();
        return;
    }

    OnlineUserId aidUsers[MAX_USERS_PER_PRESENCE_QUERY];
    uint32 uNumUsers = 0;

    // (4073)
    for (PresenceFriend & tFriend : m_tFriends)
    {
        if (uNumUsers >= MAX_USERS_PER_PRESENCE_QUERY)
        {
            break;
        }

        // (4081)
        if (tFriend.m_bSubscribedTo && (!tFriend.IsValid() || !tFriend.IsInContext()))
        {
            aidUsers[uNumUsers] = tFriend.m_idFriend;
            uNumUsers++;
        }
    }

    // (4093)
    if (uNumUsers > 0)
    {
        ::new UnsubscribeFromUsersTask(LobbyService::Get(m_nUserIndex), this, aidUsers, uNumUsers);
    }
}










uint32 PresenceGetFriends(int32 nUserIndex, Friend * ptFriends, uint32 uMaxFriends)
{
    // (4110)
    if (nUserIndex != -1)
    {
        return PresenceManager::Get(nUserIndex)->GetAllFriends(ptFriends, uMaxFriends);
    }
    return 0;
}







BOOL PresenceHasAnyFriends(int32 nUserIndex)
{
    // (4124)
    return PresenceManager::Get(nUserIndex)->HasAnyFriends();
}




uint32 PresenceGetOnlineFriends(int32 nUserIndex, Friend * ptFriends, uint32 uMaxFriends)
{
    // (4135)
    if (nUserIndex != -1)
    {
        return PresenceManager::Get(nUserIndex)->GetOnlineFriends(ptFriends, uMaxFriends);
    }
    return 0;
}







blz::vector<unsigned long> PresenceGetFriendIDs(int32 nUserIndex)
{
    // (4149)
    if (nUserIndex != -1)
    {
        return PresenceManager::Get(nUserIndex)->PresenceGetFriendIDs();
    }
    return blz::vector<unsigned long>();
}





BOOL PresenceGetRandomFriend(int32 nUserIndex, Friend & tOutFriend, bdOnlineUserID idBlacklistPlayer)
{
    // (4165)
    return PresenceManager::Get(nUserIndex)->GetRandomFriend(tOutFriend, idBlacklistPlayer);
}






BOOL PresenceGetFriendParty(int32 nUserIndex, OnlineUserId idFriend, PartyInfo & tPartyInfo)
{
    if (nUserIndex == -1)
    {
        return FALSE;
    }

    PresenceManagerInternal * ptPresenceManager = static_cast<PresenceManagerInternal *>(PresenceManager::Get(nUserIndex));
    Blizzard::Lock::ScopedRMutex tLock(ptPresenceManager->m_tMutex);

    PresenceFriend * ptFriend = ptPresenceManager->m_tFriends.FindFriend(idFriend);
    if (ptFriend && ptFriend->m_bInGame)
    {
        tPartyInfo.m_bIsOnline = ptFriend->m_tGamePresence.m_eOnlineState > PARTY_LAN;
        Blizzard::String::Copy(tPartyInfo.m_szName, ptFriend->m_szUsername, sizeof(tPartyInfo.m_szName));
        tPartyInfo.m_ptAddress = ptFriend->m_tHostAddr.m_addr;
        tPartyInfo.m_tSecID = ptFriend->m_tHostAddr.m_id;
        tPartyInfo.m_tSecKey = ptFriend->m_tHostAddr.m_key;

        Blizzard::String::Copy(tPartyInfo.m_tGameInfo.m_szUsername, ptFriend->m_szUsername, sizeof(tPartyInfo.m_tGameInfo.m_szUsername));
        tPartyInfo.m_tGameInfo.m_tPlayers[0].m_idUser = idFriend;
        tPartyInfo.m_tGameInfo.m_tPlayers[0].m_tUserPresence = ptFriend->m_tUserPresence;
        tPartyInfo.m_tGameInfo.m_eOnlineState = ptFriend->m_tGamePresence.m_eOnlineState;
        tPartyInfo.m_tGameInfo.m_bInGame = ptFriend->m_tGamePresence.m_bInGame;
        tPartyInfo.m_tGameInfo.m_bIsChallenge = ptFriend->m_tGamePresence.m_bIsChallenge;
        tPartyInfo.m_tGameInfo.m_bIsHardcore = ptFriend->m_tGamePresence.m_bIsHardcore;
        tPartyInfo.m_tGameInfo.m_bIsSeasonal = ptFriend->m_tGamePresence.m_bIsSeasonal;
        tPartyInfo.m_tGameInfo.m_eAct = ptFriend->m_tGamePresence.m_eAct;
        tPartyInfo.m_tGameInfo.m_snoQuest = ptFriend->m_tGamePresence.m_snoQuest;
        tPartyInfo.m_tGameInfo.m_uNumPlayers = ptFriend->m_tGamePresence.m_uNumPlayers;
        tPartyInfo.m_tGameInfo.m_nHandicap = ptFriend->m_tGamePresence.m_nHandicap;
        tPartyInfo.m_tGameInfo.m_uSeason = ptFriend->m_tGamePresence.m_uSeason;
        return TRUE;
    }

    return FALSE;
}

}  // namespace Online
}  // namespace Console
