// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/Online.h
// Subset: PartyMember and PartyGetLocalMembers, as X/Client/CGame.cpp uses them. The original
// header pulls in far more (DemonWare, protobuf, Player.h); of that only the nn::ldn constants and
// blz::ignore (blz/tuple.h, through Callback_Impl.h) show up in an including file's static
// initializer, so those headers are included here.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/optional.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/ChallengeRifts.pb.h"
#include "../../Common/Protobuf/Leaderboard.pb.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/SigmaTypes.h"

typedef uint64 OnlineUserId;
#include "../../Common/SNO/SNOFiles.h"

#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityID.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
class bdCommonAddr;
typedef bdReference<bdCommonAddr> bdCommonAddrRef;

namespace OnlineService { template <typename... Args> class SubscriptionEvent; }
namespace OnlineService { typedef uint64 HeroId; }  // X/Common/OnlineService/OnlineService.h
class bdBitBuffer;

namespace Console {
namespace Online {
struct GamePresence {  // size 0x2c
    // Online.h:297; recovered from the inlined Friend construction in
    // UIConsolePartyDisplay.cpp:444 and UIConsoleFriendsList.cpp:163.
    GamePresence() : m_eOnlineState(0), m_bInGame(FALSE), m_bIsChallenge(FALSE),
        m_bIsHardcore(FALSE), m_bIsSeasonal(FALSE), unk_14(FALSE), m_uSeason(0), m_nHandicap(0),
        m_eAct(-1), m_snoQuest(-1), m_uNumPlayers(0) {}
    void Serialize(bdBitBuffer & tBitBuffer) const;  // Online.cpp:2796
    bdBool Deserialize(bdBitBuffer & tBitBuffer);  // Online.cpp:2812
    int32 m_eOnlineState;  // 0x00
    BOOL m_bInGame;  // 0x04
    BOOL m_bIsChallenge;  // 0x08
    BOOL m_bIsHardcore;  // 0x0c
    BOOL m_bIsSeasonal;  // 0x10
    BOOL unk_14;  // 0x14; serialized after the season
    uint32 m_uSeason;  // 0x18
    int32 m_nHandicap;  // 0x1c
    int32 m_eAct;  // 0x20
    SNO m_snoQuest;  // 0x24
    uint32 m_uNumPlayers;  // 0x28
};
static_assert(sizeof(GamePresence) == 0x2c, "GamePresence");
static_assert(__builtin_offsetof(GamePresence, unk_14) == 0x14, "GamePresence flag");
static_assert(__builtin_offsetof(GamePresence, m_eAct) == 0x20, "GamePresence act");

struct UserPresence {  // size 0x34
    // Online.h:329. The name buffer is intentionally left untouched.
    UserPresence() : m_eClass(-1), m_bFemale(false), m_uLevel(0),
        m_uParagonLevel(0), m_gbidPortrait(-1) {}
    void Serialize(bdBitBuffer & tBitBuffer) const;  // Online.cpp:2849
    bdBool Deserialize(bdBitBuffer & tBitBuffer);  // Online.cpp:2861
    int32 m_eClass;  // 0x00
    bool m_bFemale;  // 0x04
    uint32 m_uLevel;  // 0x08
    uint32 m_uParagonLevel;  // 0x0c
    int32 m_gbidPortrait;  // 0x10
    char m_szName[32];  // 0x14
};
static_assert(sizeof(UserPresence) == 0x34, "UserPresence");

struct GameInfo : public Console::Online::GamePresence {  // size 0x150
    void Serialize(bdBitBuffer & tBitBuffer) const;  // Online.cpp:2760
    bdBool Deserialize(bdBitBuffer & tBitBuffer);  // Online.cpp:2776
    char m_szUsername[32];  // 0x2c
    struct Player {  // size 0x40
        OnlineUserId m_idUser;  // 0x00
        Console::Online::UserPresence m_tUserPresence;  // 0x08
    };
    Console::Online::GameInfo::Player m_tPlayers[4];  // 0x50
};
static_assert(sizeof(GameInfo) == 0x150, "GameInfo");
#line 380
struct SwitchUniqueID {  // size 0x10
    char value[16];  // 0x00; Inference: operator< / operator== (382-383) are inlined 16-byte memcmp calls; Empty (384-387) compares with zeroed local kEmpty (386).
    bool operator<(const Console::Online::SwitchUniqueID & rhs) const { return memcmp(value, rhs.value, sizeof(value)) < 0; }
    bool operator==(const Console::Online::SwitchUniqueID & rhs) const { return memcmp(value, rhs.value, sizeof(value)) == 0; }
    bool Empty() const
    {
        SwitchUniqueID kEmpty = {};
        return *this == kEmpty;
    }
};
static_assert(sizeof(SwitchUniqueID) == 0x10, "SwitchUniqueID");
#line 94
struct SwitchAdhocCustomData {  // size 0x160
    Console::Online::GameInfo m_gameInfo;  // 0x00
    Console::Online::SwitchUniqueID m_uniqueID;  // 0x150
};
static_assert(sizeof(SwitchAdhocCustomData) == 0x160, "SwitchAdhocCustomData");

struct PartyConnectionInfo {  // size 0x4b0
    PartyConnectionInfo();
    PartyConnectionInfo(bdCommonAddrRef, bdSecurityID, bdSecurityKey, BOOL);
    void Serialize(bdReference<bdBitBuffer> ptBitBuffer) const;  // Online.cpp:3102
    bdBool Deserialize(bdReference<bdBitBuffer> ptBitBuffer);  // Online.cpp:3113
    BOOL IsSameParty(const Console::Online::PartyConnectionInfo & tOtherConnection) const;
    BOOL m_bIsAdHocGame;  // 0x00
    nn::ldn::NetworkInfo m_Network;  // 0x08
    bdCommonAddrRef m_ptAddress;  // 0x488
    bdSecurityID m_tSecID;  // 0x490
    bdSecurityKey m_tSecKey;  // 0x498
    BOOL m_bIsOnline;  // 0x4a8
};
static_assert(sizeof(PartyConnectionInfo) == 0x4b0, "PartyConnectionInfo");

struct PartyInfo : public Console::Online::PartyConnectionInfo {  // size 0x630
    PartyInfo();
    char m_szName[32];  // 0x4ac
    Console::Online::GameInfo m_tGameInfo;  // 0x4d0
    Console::Online::SwitchUniqueID m_UniqueID;  // 0x620
};
static_assert(sizeof(PartyInfo) == 0x630, "PartyInfo");

struct Friend {  // size 0xa0
    // Online.h:942; both presence blocks default-construct before these clears.
    Friend() : m_idBDUser(0), m_bInGame(FALSE), m_bInPlatformParty(FALSE), m_bFavorite(FALSE) {
        SigmaMemoryClear(&m_idFriend, sizeof(m_idFriend));
        SigmaMemoryClear(m_szUsername, sizeof(m_szUsername));
    }
    BOOL HasDWAccount() const { return m_idBDUser != 0; }  // Online.h:954
    OnlineUserId m_idFriend;  // 0x00
    uint64 m_idBDUser;  // 0x08
    BOOL m_bInGame;  // 0x10
    BOOL m_bInPlatformParty;  // 0x14
    BOOL m_bFavorite;  // 0x18
    CHAR m_szUsername[32];  // 0x1c
    Console::Online::GamePresence m_tGamePresence;  // 0x3c
    Console::Online::UserPresence m_tUserPresence;  // 0x68
};
static_assert(sizeof(Friend) == 0xa0, "Friend");
}  // namespace Online
}  // namespace Console

// Online.h:53. Defined in X/Client/Console/Online.cpp.
namespace Console { namespace Online {
class ScopedNetworkThreadLock {  // size 0x1
public:
    ScopedNetworkThreadLock();
    ~ScopedNetworkThreadLock();
};
} }

// Online.h:295-332: mail types (implemented in X/Client/Console/MailService.cpp).
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
namespace D3 { namespace Console { class MailItem; } }
class bdUserAccountID;
namespace OnlineService { template <typename Sig> class Callback; }
namespace Console {
namespace Online {
enum MailType {
    MAIL_TYPE_MESSAGE = 0,
    MAIL_TYPE_EVENT = 1,
    NUM_MAIL_TYPES = 2,
    MAIL_TYPE_MESSAGE_HC = 2,
};

class Mail : public bdReferencable {  // size 0x10
public:
    virtual ~Mail() {}  // vtable slot 0
    virtual uint64 GetID() const = 0;  // vtable slot 2
    virtual bdUserAccountID GetSenderID() const = 0;  // vtable slot 3
    virtual const char * GetSender() const = 0;  // vtable slot 4
    virtual Blizzard::Time::Timestamp GetSentTime() const = 0;  // vtable slot 5
    virtual Blizzard::Time::Timestamp GetExpiration() const = 0;  // vtable slot 6
    virtual const D3::Console::MailItem & GetContents() const = 0;  // vtable slot 7
    virtual BOOL AreContentsValid() const = 0;  // vtable slot 8
    virtual BOOL IsRead() const = 0;  // vtable slot 9
    virtual BOOL IsReturned() const = 0;  // vtable slot 10
};
static_assert(sizeof(Mail) == 0x10, "Mail");

typedef bdReference<Console::Online::Mail> MailRef;

typedef blz::vector<bdReference<Console::Online::Mail>, blz::allocator<bdReference<Console::Online::Mail> > > MailArray;

enum MailResult {
    MAIL_SUCCESS = 0,
    MAIL_FAILURE = 1,
    MAIL_FAILURE_NO_ACCOUNT = 2,
    MAIL_FAILURE_NOT_CONNECTED = 3,
};

typedef OnlineService::Callback<void (Console::Online::MailResult)> SendMailCallback;

typedef OnlineService::Callback<void (Console::Online::MailResult, bdReference<Console::Online::Mail>)> DeleteMailCallback;

typedef OnlineService::Callback<void (Console::Online::MailResult, bdReference<Console::Online::Mail>)> VerifyMailCallback;

// Defined in X/Client/Console/MailService.cpp.
void MailGetAllOfType(int32 nUserIndex, MailType eType, MailArray & atMail, int32 nStart, int32 nMax);
BOOL MailHasMail(int32 nUserIndex, MailType eType);
void MailSetGameState(bool bActive, bool bIsHardcore, bool bIsSeasonal, uint32 uSeason);

// Defined in X/Client/Console/LobbyService.cpp.
Blizzard::Time::Timestamp GetNetworkTimestamp();

// Original line 266. Value parameters used by the game-settings screen.
struct GameParams {
    GameParams(); void Serialize(bdReference<bdBitBuffer> ptBitBuffer) const; bdBool Deserialize(bdReference<bdBitBuffer> ptBitBuffer);
    SNO m_snoDevWorld;
    uint32 m_dwDevCreationFlags;
    int32 m_eGameType;
    BOOL m_bIsHardcore;
    BOOL m_bIsSeasonal;
    BOOL unk_14;  // 0x14; serialized after the challenge configuration
    BOOL m_bResumeFromSave;
    uint32 m_uSeason;
    int32 m_nLevel;
    int32 m_eAct;
    SNO m_snoQuest;
    int32 m_nQuestStep;
    int32 m_nHandicap;
    int32 m_nTag;
    uint32 m_uEntranceGUIDOverride;
    int32 m_ePreferredAdventureAct;
    D3::ChallengeRifts::ChallengeData m_tWeeklyChallengeConfig;
    D3::Leaderboard::WeeklyChallengeData m_tWeeklyChallengeData;
};
static_assert(sizeof(GameParams) == 0xc0, "GameParams");
static_assert(__builtin_offsetof(GameParams, unk_14) == 0x14, "GameParams flag");
static_assert(__builtin_offsetof(GameParams, m_nHandicap) == 0x30, "GameParams handicap");
BOOL PartyIsHost();
BOOL PartyIsInParty();
BOOL PartyIsInParty(int32 nControllerIndex);  // Online.cpp:3987
BOOL PartyIsOnline();
uint32 PartyGetMemberCount();  // Online.cpp:4265
uint32 PartyGetMemberCountToJoinOnlineGame();  // Online.cpp:4300
void PartyUpdatePlayerHero(int32 nControllerIndex);
void PartyUpdateCachedRemotePlayerAccountAndHero(int32 nPartyIndex, OnlineService::HeroId idHero, const D3::Account::SavedDefinition & tAccountDefinition, const D3::Hero::SavedDefinition & tHeroDefinition);  // Online.cpp:4425
BOOL PartyIsPlayerAndHeroStillPresent(int32 nPartyIndex, OnlineService::HeroId idHero);  // Online.cpp:4496
BOOL PartyGetGameParams(GameParams & tParams);
void PartySetGameParams(const GameParams & tParams);
enum {
    PARTY_OFFLINE = 0,
    PARTY_ADHOC = 1,
    PARTY_LAN = 2,
    PARTY_ONLINE = 3,
    PARTY_ONLINE_INVITEONLY = 3,
    PARTY_ONLINE_FRIENDSONLY = 4,
    PARTY_ONLINE_PUBLIC = 5,
    NUM_PARTY_ONLINE_STATES = 6,
};
typedef int32 PartyOnlineState;
enum PartySwitchOnlineResult {
    PARTY_SWITCH_ONLINE_OK = 0,
    PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_LOCALLY = 1,
    PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_ONLINE = 2,
    PARTY_SWITCH_ONLINE_NO_PRIVILEGE = 3,
    PARTY_SWITCH_ONLINE_PARENTAL_BLOCK = 4,
    PARTY_SWITCH_ONLINE_BANNED = 5,
    PARTY_SWITCH_ONLINE_PREMIUM_SERVICE_REQUIRED = 6,
    PARTY_SWITCH_ONLINE_OTHER_XBOXONE_ISSUE = 7,
    PARTY_SWITCH_ONLINE_WILL_DISCONNECT = 8,
    PARTY_SWITCH_ONLINE_UPDATE_REQUIRED = 9,
    PARTY_SWITCH_NO_NETWORK_CONNECTION = 10,
    PARTY_SWITCH_NO_INTERNET_CONNECTION = 11,
    PARTY_SWITCH_UNKNOWN_ERROR = 12,
};
PartyOnlineState PartyGetOnlineState();  // Online.cpp:4628
PartySwitchOnlineResult PartyCanSwitchOnlineState(PartyOnlineState eState);  // Online.cpp:4659
void PartySwitchPreferredOnlineState(PartyOnlineState eState);  // Online.cpp:4763
struct PartyMember {  // size 0x1d8
    PartyMember();
    uint8 idPlayer;  // 0x00
    OnlineUserId idOnlineUser;  // 0x08
    char szName[128];  // 0x10
    BOOL bIsLocal;  // 0x90
    BOOL bIsPrimary;  // 0x94
    BOOL bIsGuest;  // 0x98
    BOOL bIsInGame;  // 0x9c
    int32 nControllerIndex;  // 0xa0
    blz::optional<D3::Hero::Digest> tHeroDigest;  // 0xa8
    blz::optional<D3::Account::Digest> tAccountDigest;  // 0x158
    void Copy(Console::Online::PartyMember & tOther, BOOL bDigests) const;  // Online.h:685
};
static_assert(sizeof(PartyMember) == 0x1d8, "PartyMember");

// Online.h:673. The body is not in the DWARF; written from the copies inlined into
// X/Client/CGame.cpp (sConnectedToGameServer), lines 674-682.
inline PartyMember::PartyMember()
    : idPlayer(0), bIsLocal(FALSE), bIsPrimary(FALSE), bIsGuest(FALSE), bIsInGame(FALSE), nControllerIndex(-1) {
    SigmaMemoryClear(&idOnlineUser, sizeof(idOnlineUser));
    szName[0] = '\0';
}

// Online.h:685. Body from the line table of the out-of-line copy in Online.cpp (0x7100052a50):
// 687 idPlayer, 688 idOnlineUser, 689 strncpy of 0x80 bytes, 690 terminator at [127], 691-695 the
// flags and controller index, 696 bDigests test, 698/699 the two optional digests.
inline void PartyMember::Copy(PartyMember & tOther, BOOL bDigests) const {
    tOther.idPlayer = idPlayer;
    tOther.idOnlineUser = idOnlineUser;
    strncpy(tOther.szName, szName, sizeof(tOther.szName));
    tOther.szName[sizeof(tOther.szName) - 1] = '\0';
    tOther.bIsLocal = bIsLocal;
    tOther.bIsPrimary = bIsPrimary;
    tOther.bIsGuest = bIsGuest;
    tOther.bIsInGame = bIsInGame;
    tOther.nControllerIndex = nControllerIndex;
    if (bDigests) {
        tOther.tHeroDigest = tHeroDigest;
        tOther.tAccountDigest = tAccountDigest;
    }
}

// Defined in X/Client/Console/Online.cpp.
uint32 PartyGetMembers(PartyMember * ptMembers, int32 nFlags, BOOL bDigests);

inline uint32 PartyGetLocalMembers(PartyMember * ptMembers, BOOL bDigests) {  // line 723
    return PartyGetMembers(ptMembers, 0, bDigests);
}
// Online.h:391, Online.cpp:5210. SubscriptionEvent is declared below so users
// of roster/presence data do not need the callback implementation.
typedef OnlineService::SubscriptionEvent<int> PartyMemberRemovedEvent;
PartyMemberRemovedEvent & GetPartyMemberRemovedEvent();
}  // namespace Online
}  // namespace Console

// Online.cpp: the app subsystem entry points and the per-frame idle (X/Common/Game.cpp).
namespace Console { namespace Online {
// Online.h: leaderboard request results.
enum class LeaderboardResult : int {
    SUCCESS = 0,
    FAILED = 1,
    FAILED_NOT_CONNECTED = 2,
};
void InitializeForApp();
void DisposeForApp();
void Idle();
} }

// Online.h: party join/migration types used by X/Client/Console/ConsoleParty.h.
namespace OnlineService { template <typename Sig> class Callback; }
namespace Console { namespace Online {
enum JoinType {
    JOIN_TYPE_NORMAL = 0,
    JOIN_TYPE_MATCHMAKE = 1,
    JOIN_TYPE_INVITE = 2,
    JOIN_TYPE_MERGE = 3,
    NUM_JOIN_TYPES = 4,
};
enum JoinResult {
    JOIN_SUCCESS = 0,
    JOIN_CANCELLED = 1,
    JOIN_FAILED_NO_PRIVILEGE = 2,
    JOIN_FAILED_UNABLE_TO_CONNECT = 3,
    JOIN_FAILED_TIMEOUT = 4,
    JOIN_FAILED_GAME_FULL = 5,
    JOIN_FAILED_REQUIRES_UPDATE = 6,
    JOIN_FAILED_HOST_VERSION_TOO_OLD = 7,
    JOIN_FAILED_PAUSED_DURING_MATCHMAKE = 8,
    JOIN_FAILED_INVITE_ONLY = 9,
    JOIN_FAILED = 10,
    NUM_JOIN_RESULTS = 11,
};
typedef OnlineService::Callback<void (Console::Online::JoinResult)> PartyJoinCallback;
typedef OnlineService::Callback<void (int)> PartyAddPlayerCallback;
enum MigrationEvent {
    MIGRATION_STARTED = 0,
    MIGRATION_FINISHED = 1,
    MIGRATION_NOT_POSSIBLE = 2,
};
} }
// Online.cpp:595.
BOOL IsSameAddress(bdCommonAddrRef ptAddr1, bdCommonAddrRef ptAddr2);
// ConsoleGameConnection.cpp:180, 211.
namespace Console { namespace Online {
void GameClearServerPendingConnections();
void GameMarkServerGameConnectionsDontClose();
} }
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
namespace Console { namespace Online {
// Online.h:74; constructor at Online.cpp:418.
class ScopedNetworkSendLock : private Blizzard::Lock::ScopedRMutex {  // size 0x8
public:
    ScopedNetworkSendLock();
};
} }
#include "../../Common/OnlineService/SubscriptionEvent.h"
namespace Console { namespace Online {
typedef OnlineService::Callback<void ()> OnlineEventCallback;
typedef OnlineService::SubscriptionEvent<Console::Online::MigrationEvent> PartyMigratingEvent;
typedef OnlineService::SubscriptionEvent<Console::Online::PartySwitchOnlineResult> PartyOnlineStateChangedEvent;
void QueueNetworkEventForMainThread(OnlineEventCallback pfnCallback);  // Online.cpp:274
PartyMigratingEvent & GetPartyMigratingEvent();  // Online.cpp:5250
PartyOnlineStateChangedEvent & GetPartyOnlineStateChangedEvent();  // Online.cpp:5258

// Online.h:1375 (variadic in the original; one argument is all the callers so far need).
template <typename A1>
inline void QueueNotification(OnlineService::SubscriptionEvent<A1> & tEvent, A1 args) {
    QueueNetworkEventForMainThread(
        OnlineService::internal::method_binder_1<void, OnlineService::SubscriptionEvent<A1>, A1>::template bind<
            &OnlineService::SubscriptionEvent<A1>::NotifySubscribers>(&tEvent, args));  // line 1380
}
template <typename A1, typename A2>
inline void QueueNotification(OnlineService::SubscriptionEvent<A1, A2> & tEvent, A1 arg1, A2 arg2) {
    QueueNetworkEventForMainThread(
        OnlineService::internal::method_binder_2<void, OnlineService::SubscriptionEvent<A1, A2>, A1, A2>::template bind<
            &OnlineService::SubscriptionEvent<A1, A2>::NotifySubscribers>(&tEvent, arg1, arg2));  // line 1380
}
} }
class bdConnection;
namespace Console { namespace GameNetwork { class Connection; typedef bdReference<Console::GameNetwork::Connection> ConnectionRef; } }
namespace Console { namespace Online {
Console::GameNetwork::ConnectionRef GameGetGameConnection(bdReference<bdConnection> ptConnection);  // ConsoleGameConnection.cpp:227
} }
namespace Console { namespace Online {
typedef OnlineService::SubscriptionEvent<> PartyJoinedEvent;
PartyJoinedEvent & GetPartyJoinedEvent();  // Online.cpp:5226
} }
namespace Console { namespace Online {
// Online.h:1375 with no arguments.
inline void QueueNotification(OnlineService::SubscriptionEvent<> & tEvent) {
    QueueNetworkEventForMainThread(
        OnlineService::internal::method_binder_0<void, OnlineService::SubscriptionEvent<> >::template bind<
            &OnlineService::SubscriptionEvent<>::NotifySubscribers>(&tEvent));
}
} }
namespace Console { namespace Online {
void LANUpdateGameInfo(const char * szName, const bdSecurityID & tSecID, const bdSecurityKey & tSecKey, const GameInfo & tGameInfo);
} }
// Online.h:80 and the event typedefs of Online.h:390-437, for X/Client/Console/Online.cpp.
namespace Console { namespace Online {
enum NetworkMode {
    NET_OFFLINE = 0,
    NET_ADHOC = 1,
    NET_LAN = 2,
    NET_ONLINE = 3,
};
enum MMSearchResult {
    MMSEARCH_SUCCESS = 0,
    MMSEARCH_CANCELLED = 1,
    MMSEARCH_DISCONNECTED = 2,
    MMSEARCH_FAILED = 3,
};
typedef OnlineService::Callback<void (Console::Online::MMSearchResult, unsigned int)> PartyFindCallback;
enum PSNSignInStatus {
    PSN_SIGNED_IN = 0,
    PSN_NOT_SIGNED_IN = 1,
};
enum PhysicalNetworkStatus {
    PHYSICAL_NETWORK_STATUS_NOT_CONNECTED = 0,
    PHYSICAL_NETWORK_STATUS_CONNECTED = 1,
};
enum LobbyConnectionEventType {
    LOBBY_EVENT_CONNECTED = 0,
    LOBBY_EVENT_DISCONNECTED = 1,
};
typedef OnlineService::SubscriptionEvent<int> PartyMemberAddedEvent;
typedef OnlineService::SubscriptionEvent<int> PartyMemberReceivedDigestEvent;
typedef OnlineService::SubscriptionEvent<int> PartyInviteAcceptedEvent;
typedef OnlineService::SubscriptionEvent<> PartyGameInfoChangedEvent;
typedef OnlineService::SubscriptionEvent<int> PartyMemberVoiceChatTalkingEvent;
typedef OnlineService::SubscriptionEvent<int, Console::Online::PSNSignInStatus> PSNSignInChangedEvent;
typedef OnlineService::SubscriptionEvent<Console::Online::PhysicalNetworkStatus> PhysicalNetworkStatusChangedEvent;
typedef OnlineService::SubscriptionEvent<> GameStartingEvent;
typedef OnlineService::SubscriptionEvent<> GameEndingEvent;
typedef OnlineService::SubscriptionEvent<int, Console::Online::LobbyConnectionEventType> LobbyConnectionEvent;
typedef OnlineService::SubscriptionEvent<int> NewMailEvent;
typedef OnlineService::SubscriptionEvent<int> PlayTogetherEvent;
typedef OnlineService::SubscriptionEvent<> SeasonsChangedEvent;
PSNSignInChangedEvent & GetPSNSignInChangedEvent();  // Online.cpp:5266
PhysicalNetworkStatusChangedEvent & GetPhysicalNetworkStatusChangedEvent();  // Online.cpp:5274
LobbyConnectionEvent & GetLobbyConnectionEvent();  // Online.cpp:5298
PartyMemberAddedEvent & GetPartyMemberAddedEvent();  // Online.cpp
NetworkMode GetNetworkMode();  // Online.cpp:335
BOOL IsNetworkChanging();  // Online.cpp:336
BOOL IsNetworkRunning();  // Online.cpp:337
BOOL IsPhysicalNetworkConnected();  // Online.cpp:1257
} }
// Online.h: storage / leaderboard / linked-account results and their callbacks
// (X/Client/Console/LobbyService.cpp).
namespace blz { template <typename T> class shared_ptr; }
class bdContextUserStorageFileInfo;
class bdLinkedAccountsResult;
namespace Console { namespace Online {
typedef OnlineService::Callback<void (int)> IsSeasonInProgressCallback;
enum StorageResult {
    STORAGE_SUCCESS = 0,
    STORAGE_FAILED = 1,
    STORAGE_FAILED_NOT_CONNECTED = 2,
    STORAGE_FAILED_FILE_NOT_FOUND = 3,
    STORAGE_FAILED_DATA_INVALID = 4,
};
typedef OnlineService::Callback<void (Console::Online::StorageResult, blz::shared_ptr<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > >)> StorageGetFileCallback;
typedef OnlineService::Callback<void (Console::Online::StorageResult, bdContextUserStorageFileInfo)> StorageFindUserFileCallback;
typedef OnlineService::Callback<void (Console::Online::StorageResult)> StoragePublishUserFileCallback;
typedef OnlineService::Callback<void (Console::Online::LeaderboardResult)> LeaderboardPostCallback;
typedef OnlineService::Callback<void (Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > &)> LeaderboardGetScoresCallback;
enum LinkedAccountsResult {
    LINKED_ACCOUNTS_SUCCESS = 0,
    LINKED_ACCOUNTS_FAILED = 1,
    LINKED_ACCOUNTS_FAILED_NOT_CONNECTED = 2,
};
typedef OnlineService::Callback<void (Console::Online::LinkedAccountsResult, bdLinkedAccountsResult)> LinkedAccountsSearchCallback;
} }

// Online.cpp:5306
namespace Console { namespace Online {
typedef OnlineService::SubscriptionEvent<int> NewMailEvent;
NewMailEvent & GetNewMailEvent();
} }  // namespace Console::Online

#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
namespace Console { namespace Online {
bdNATType GetNATType();  // Online.cpp:2417
} }

namespace Console { namespace Online {
void sSynchronizePlayerHeroes();  // Defined in Online.cpp:2277.
} }

namespace Console { namespace Online {
uint64 GetTitleVersion();  // Online.cpp:184
BOOL CheckVersionCompatability(uint64 uVersion);  // Online.cpp:194
} }

namespace Console { namespace Online {
void PartyRemovePlayer(int32 nControllerIndex);  // Online.cpp:3766
} }

namespace Console { namespace Online {
void SwitchToNetworkMode(NetworkMode eMode, BOOL bForce);
} }

namespace Console { namespace Online {
#line 746 "X/Client/Console/Online.h"
enum { PLAYERID_HOST = 255 };
} }




































































































































































































































































































































































































































































































































































































































































































































































































































#include "../../Common/GameSwig.h"

namespace Console { namespace Online {
struct DummySetupParams { // DWARF Online.h:1325, size 0x20.

    DummySetupParams()  // Online.h:1331-1335, inlined into MainNX64.cpp sMain
    {
        m_snoWorld = INVALID_SNO;
        m_dwCreationFlags = 0;
        m_dwLocalPlayerCount = 1;
        m_nHandicap = 0;
        for (int32 i = 0; i < 4; i++)
            m_playerClasses[i] = -1;
    }



    SNO m_snoWorld;
    uint32 m_dwCreationFlags;
    uint32 m_dwLocalPlayerCount;
    int32 m_nHandicap;
    PlayerClass m_playerClasses[4];
};
static_assert(sizeof(DummySetupParams) == 0x20, "DummySetupParams");
static_assert(offsetof(DummySetupParams, m_playerClasses) == 0x10, "DummySetupParams::m_playerClasses");
} }

namespace Console { namespace Online {
// DWARF Online.h:149, constructors/declarations:152/153; bodies Online.cpp:406/411.
class ScopedAdHocOperationThreadLock {
public:
    ScopedAdHocOperationThreadLock();
    ~ScopedAdHocOperationThreadLock();
};
static_assert(sizeof(ScopedAdHocOperationThreadLock) == 1, "ScopedAdHocOperationThreadLock");
} }
