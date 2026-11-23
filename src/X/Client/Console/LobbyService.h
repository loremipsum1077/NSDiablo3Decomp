// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/LobbyService.h
// Subset: LobbyService (the full vtable, so slot offsets are right) and LobbyTask. Parameter
// types of the slots nobody in src/ calls yet are only forward-declared.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTask.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Online.h"
#include "TaskManager.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/SubscriptionEvent.h"
typedef bdReference<bdRemoteTask> bdRemoteTaskRef;  // as in the bdLobby service headers

struct LeaderboardKeyAndPartition;
class bdDML;
class bdEventLog;
class bdLobbyService;
class bdMail;
class bdMatchMaking;
class bdMessaging;
class bdRichPresenceService;
class bdStats;
class bdStorage;
class bdContextUserStorageFileInfo;
typedef bdContextUserStorageFileInfo bdUserStorageFileInfo;
class bdTimeStamp;

namespace Console {
namespace Online {
class LeaderboardStatsInfo;

enum GlobalCounter {
    GLOBAL_COUNTER_NUM_LOCAL_PLAYERS_IN_GAME_0 = 0,
    GLOBAL_COUNTER_NUM_LOCAL_PLAYERS_IN_GAME_END = 4,
};

typedef OnlineService::Callback<void ()> VerifyNSOCallback;

class LobbyService {  // size 0x8
    // vtable pointer
public:
    static void Create(uint32 idTitle);
    static Console::Online::LobbyService * Get(int32 nUserIdx);
    static void Idle();
    static void ConnectAll();
    static void DisconnectAll();
    static void AttemptConnectOnUser(int32 nUserIndex);
    static BOOL IsUserActive(int32 nUserIndex);
    static BOOL IsUserConnected(int32 nUserIndex);
    static BOOL ShouldShowReconnectOption(int32 nUserIndex);
    static void RunTokenAuthentication(int32 nUserIndex);
    static void VerifyNSOSubscriptionAsync(int32 nUserIndex, Console::Online::VerifyNSOCallback pfnCallback);
    static void DisconnectUser(int32);
    static BOOL IsAnyActive();
    virtual void Connect() = 0;  // vtable slot 0
    virtual void Disconnect() = 0;  // vtable slot 1
    virtual BOOL IsActive() = 0;  // vtable slot 2
    virtual BOOL IsConnected() = 0;  // vtable slot 3
    virtual BOOL IsConnecting() = 0;  // vtable slot 4
    virtual bdOnlineUserID GetOnlineUserID() = 0;  // vtable slot 5
    virtual Console::Online::TaskManager & GetTaskManager() = 0;  // vtable slot 6
    virtual bdLobbyService * GetLobbyService() = 0;  // vtable slot 7
    virtual bdMatchMaking * GetMatchMaking() = 0;  // vtable slot 8
    virtual bdStorage * GetStorage() = 0;  // vtable slot 9
    virtual bdStorage * GetStorage(const char *, bdUserID &) = 0;  // vtable slot 10
    virtual bdMail * GetMail() = 0;  // vtable slot 11
    virtual bdStats * GetStats() = 0;  // vtable slot 12
    virtual bdEventLog * GetEventLog() = 0;  // vtable slot 13
    virtual bdMessaging * GetMessaging() = 0;  // vtable slot 14
    virtual bdRichPresenceService * GetRichPresence() = 0;  // vtable slot 15
    virtual bdDML * GetDML() = 0;  // vtable slot 16
    virtual void StorageGetPublisherFile(const char *, Console::Online::StorageGetFileCallback, bool) = 0;  // vtable slot 17
    virtual void StorageGetUserFile(const char *, const bdUserStorageFileInfo &, Console::Online::StorageGetFileCallback) = 0;  // vtable slot 18
    virtual void StorageFindUserFile(bdUserAccountID, const char *, const char *, Console::Online::StorageFindUserFileCallback) = 0;  // vtable slot 19
    virtual void StoragePublishUserFile(const char *, const char *, blz::string, Console::Online::StoragePublishUserFileCallback) = 0;  // vtable slot 20
    virtual void LeaderboardWriteStats(blz::vector<Console::Online::LeaderboardStatsInfo, blz::allocator<Console::Online::LeaderboardStatsInfo> > &, Console::Online::LeaderboardPostCallback) = 0;  // vtable slot 21
    typedef OnlineService::Callback<void (Console::Online::LeaderboardResult, const blz::vector<Console::Online::LeaderboardStatsInfo, blz::allocator<Console::Online::LeaderboardStatsInfo> > &)> LeaderboardReadStatsCallback;
    virtual void LeaderboardReadStatsByLeaderboardIds(const blz::vector<LeaderboardKeyAndPartition, blz::allocator<LeaderboardKeyAndPartition> > &, Console::Online::LobbyService::LeaderboardReadStatsCallback) = 0;  // vtable slot 22
    virtual void LeaderboardReadStatsByPivot(const LeaderboardKeyAndPartition &, uint32, Console::Online::LobbyService::LeaderboardReadStatsCallback) = 0;  // vtable slot 23
    virtual void LeaderboardReadStatsByEntityId(const LeaderboardKeyAndPartition &, const blz::vector<unsigned long, blz::allocator<unsigned long> > &, bool, Console::Online::LobbyService::LeaderboardReadStatsCallback) = 0;  // vtable slot 24
    virtual void LeaderboardReadStatsByRank(const LeaderboardKeyAndPartition &, uint32, uint32, Console::Online::LobbyService::LeaderboardReadStatsCallback) = 0;  // vtable slot 25
    virtual void LeaderboardReadMetadata(const LeaderboardKeyAndPartition &, bdOnlineUserID, Console::Online::LobbyService::LeaderboardReadStatsCallback) = 0;  // vtable slot 26
    typedef OnlineService::Callback<void (int, unsigned long *, unsigned long *, unsigned int)> LookupUserAccountsCallback;
    virtual void LookupUserAccounts(OnlineUserId *, uint32, Console::Online::LobbyService::LookupUserAccountsCallback) = 0;  // vtable slot 27
    virtual void GetLinkedAccounts(Console::Online::LinkedAccountsSearchCallback) = 0;  // vtable slot 28
    typedef blz::vector<blz::pair<Console::Online::GlobalCounter, int>, blz::allocator<blz::pair<Console::Online::GlobalCounter, int> > > CounterVector;
    virtual void UpdateGlobalCounters(const Console::Online::LobbyService::CounterVector &) = 0;  // vtable slot 29
    typedef OnlineService::Callback<void (int, const bdTimeStamp &)> GetServerTimeCallback;
    virtual void GetServerTime(Console::Online::LobbyService::GetServerTimeCallback, bool) = 0;  // vtable slot 30
    virtual BOOL SendGlobalInstantMessage(OnlineUserId, const void *const, const bdUInt) = 0;  // vtable slot 31
    typedef OnlineService::SubscriptionEvent<int> ConnectEvent;
    virtual Console::Online::LobbyService::ConnectEvent & GetConnectEvent() = 0;  // vtable slot 32
    typedef OnlineService::SubscriptionEvent<int> DisconnectEvent;
    virtual Console::Online::LobbyService::DisconnectEvent & GetDisconnectEvent() = 0;  // vtable slot 33
    typedef OnlineService::SubscriptionEvent<> NewMailEvent;
    virtual Console::Online::LobbyService::NewMailEvent & GetNewMailEvent() = 0;  // vtable slot 34
    static bdOnlineUserID CreateBDOnlineUserID(OnlineUserId);
};
static_assert(sizeof(LobbyService) == 0x8, "LobbyService");

class LobbyTask : public Console::Online::Task {  // size 0x20
public:
    LobbyTask(Console::Online::LobbyService *);
    virtual ~LobbyTask() {}  // line 256
    virtual void Cancel();  // vtable slot 3
    virtual Console::Online::Task::Status GetStatus();  // vtable slot 4
    Console::Online::LobbyService * m_ptLobbyService;  // 0x10
    bdRemoteTaskRef m_ptRemoteTask;  // 0x18
};
static_assert(sizeof(LobbyTask) == 0x20, "LobbyTask");

typedef bdReference<Console::Online::LobbyTask> LobbyTaskRef;

}  // namespace Online
}  // namespace Console

namespace Console { namespace Online {
uint32 GetSecondsUntilSeasonEnd();  // Defined in LobbyService.cpp:781.
} }
