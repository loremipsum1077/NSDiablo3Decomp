// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/ConsoleParty.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnection.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnectionListener.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdDispatch/bdDispatchInterceptor.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityID.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdString/bdString.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Types.h"
#include "Online.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Sigma/BlzExt/fixed_vector.h"
#include <stddef.h>
#include <stdint.h>

namespace Console { namespace NX64 { class SessionManager; } }
namespace Console { namespace Online { struct MatchParameters; } }
namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace Hero { class Digest; } }
namespace D3 { namespace Hero { class SavedDefinition; } }
class bdReceivedMessage;
namespace google { namespace protobuf { class Message; } }

namespace Console {
namespace Online {
class Party;
struct D3PartyJoinRequest;
struct D3PartyJoinResponse;
struct D3PartyMsgAddPlayerRequest;

enum {
    D3_GAMESTREAM_SERVER = 29,
    D3_GAMESTREAM_CLIENT = 30,
    D3_MSG_PARTY = 31,
    D3_MSG_PARTY_SESSIONINFO = 31,
    D3_MSG_PARTY_JOIN_REQ = 32,
    D3_MSG_PARTY_JOIN_RSP = 33,
    D3_MSG_PARTY_LEAVING = 34,
    D3_MSG_PARTY_PEER_HELLO = 35,
    D3_MSG_PARTY_MERGE = 36,
    D3_MSG_PARTY_INFO = 37,
    D3_MSG_PARTY_ADDPLAYER_REQ = 38,
    D3_MSG_PARTY_ADDPLAYER_RSP = 39,
    D3_MSG_PARTY_REMOVEPLAYER = 40,
    D3_MSG_PARTY_UPDATEPLAYERS = 41,
    D3_MSG_PARTY_CHUNKED_DATA = 42,
    D3_MSG_PARTY_START_GAME = 43,
    D3_MSG_PARTY_REQUEST_START_GAME = 44,
    D3_MSG_PARTY_GAME_STARTING = 45,
    D3_MSG_PARTY_GAME_ENDING = 46,
    D3_MSG_PARTY_PLAYER_IN_GAME = 47,
    D3_MSG_PARTY_MIGRATE = 48,
    D3_MSG_PARTY_VALIDATE_REQ = 49,
    D3_MSG_PARTY_VALIDATE_RSP = 50,
    D3_MSG_PARTY_IAMHOST = 51,
    D3_MSG_PARTY_VOICECHAT_DATA = 52,
    D3_MSG_PARTY_VOICECHAT_MUTEUPDATE = 53,
    D3_MSG_PARTY_VOICECHAT_MICDETECTED = 54,
    D3_MSG_PARTY_VOICECHAT_RELIABLE_DATA = 55,
};

enum PartyChunkedDataType {
    CHUNKED_DATA_INVALID = 0,
    CHUNKED_DATA_ACCOUNT = 1,
    CHUNKED_DATA_HERO = 2,
    CHUNKED_DATA_HERO_DIGEST = 3,
    CHUNKED_DATA_CHALLENGE_RIFT = 4,
    CHUNKED_DATA_MAX = 5,
};

enum PlayerType {
    PLAYER_REGULAR = 0,
    PLAYER_PRIMARY = 1,
    PLAYER_GUEST = 2,
    NUM_PLAYER_TYPES = 3,
};

enum PacketResult {
    PACKET_OK = 0,
    PACKET_CORRUPT = 1,
    PACKET_INVALID = 2,
    PACKET_NOT_HANDLED = 3,
};

enum {
    MAX_FRAGMENT_SIZE = 1235,
    MAX_CHUNK_SIZE = 1227,
    MAX_CHUNKED_BUFFER_SIZE = 262144,
    MAX_CHUNKED_UNCOMPRESSED_SIZE = 524288,
    MAX_PEERS = 4,
};

struct AdditionalPlatformInformation {  // size 0x40
    nn::friends::PlayHistoryRegistrationKey m_tPlayRegistrationKey;  // 0x00
};
static_assert(sizeof(AdditionalPlatformInformation) == 0x40, "AdditionalPlatformInformation");

class PartyPlayer {  // size 0xe0
public:
    PartyPlayer();
    ~PartyPlayer();
    void Set(uint8 idPlayer, bdCommonAddrRef ptAddr, int32 nControllerIndex, const char * szName, const Console::Online::AdditionalPlatformInformation & tAdditionalPlatformInfo, OnlineUserId & idOnlineUser, Console::Online::PlayerType ePlayerType, int32 nSponsorIndex, BOOL bVoiceActive, BOOL bBadReputation);
    void Clear();
    BOOL IsReserved() { return m_uReservationTime != 0; }  // line 157
    void StartReservation();
    void ClearReservation();
    void ReleaseChunkedData();
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdCommonAddrRef me, bdBitBufferRef ptBitBuffer);
    BOOL IsValid() const { return m_idPlayer != -1; }  // line 179
    BOOL IsLocal() const;
    BOOL IsLocal(int32 nControllerIndex) const;
    BOOL IsAddressAndIndex(const bdCommonAddrRef ptAddr, int32 nControllerIndex) const;
    BOOL IsSamePlayer(const Console::Online::PartyPlayer & tPlayer) const;
    Console::Online::PlayerType GetType() const;
    BOOL IsPrimary() const { return m_ePlayerType == PLAYER_PRIMARY; }  // line 204
    BOOL IsGuest() const { return m_ePlayerType == PLAYER_GUEST; }  // line 210
    BOOL IsInGame() const { return m_bInGame; }  // line 216
    void UpdateAccountAndHero();
    const D3::Account::SavedDefinition * GetAccount() const;
    const D3::Hero::SavedDefinition * GetHero() const;
    const D3::Hero::Digest * GetHeroDigest() const;
    void Export(Console::Online::PartyMember & tMember, BOOL bDigests);
    bdByte8 m_idPlayer;  // 0x00
    bdCommonAddrRef m_ptAddr;  // 0x08
    bdInt32 m_nControllerIndex;  // 0x10
    Console::Online::PlayerType m_ePlayerType;  // 0x14
    bdInt32 m_nSponsorIndex;  // 0x18
    bdBool m_bVoiceActive;  // 0x1c
    bdBool m_bBadReputation;  // 0x1d
    char m_szName[32];  // 0x1e
    Console::Online::AdditionalPlatformInformation m_tAdditionalPlatformInfo;  // 0x3e
    OnlineUserId m_idOnlineUser;  // 0x80
    Blizzard::Time::Millisecond m_uReservationTime;  // 0x88
    bdBool m_bInGame;  // 0x8c
    BOOL m_bIsLeaving;  // 0x90
    D3::Account::SavedDefinition * m_ptAccountDefinition;  // 0x98
    D3::Hero::SavedDefinition * m_ptHeroDefinition;  // 0xa0
    D3::Hero::Digest * m_ptHeroDigest;  // 0xa8
    blz::string m_szChunkDataBuffer;  // 0xb0
    uint32 m_uChunkUncompressedSize;  // 0xd8
    Console::Online::PartyChunkedDataType m_eChunkType;  // 0xdc
};
static_assert(sizeof(PartyPlayer) == 0xe0, "PartyPlayer");

class Peer : public bdReferencable {  // size 0x20
public:
    ~Peer() {}  // line 267
    BD_CLASS_ALLOCATOR(Peer);  // line 270
    Peer(bdConnectionRef ptConnection, bdNATType eNATType)  // line 272
        : m_idPeer(++ms_idTopPeer), m_ptConnection(ptConnection), m_eNATType(eNATType), m_bHasRequestedMigration(FALSE) {}
    int32 m_idPeer;  // 0x0c
    const bdConnectionRef m_ptConnection;  // 0x10
    const bdNATType m_eNATType;  // 0x18
    BOOL m_bHasRequestedMigration;  // 0x1c
private:
    Peer(const Console::Online::Peer &);
    void operator=(const Console::Online::Peer &);
    static int32 ms_idTopPeer;
};
static_assert(sizeof(Peer) == 0x20, "Peer");

typedef bdReference<Console::Online::Peer> PeerRef;

struct PartyPendingJoinInfo : public Console::Online::PartyConnectionInfo {  // size 0x4c0
    PartyPendingJoinInfo(int32, Console::Online::JoinType);
    PartyPendingJoinInfo(Console::Online::PartyConnectionInfo &, int32, Console::Online::JoinType, Console::Online::PartyJoinCallback);
    int32 m_nControllerIndex;  // 0x4ac
    Console::Online::JoinType m_eJoinType;  // 0x4b0
    Console::Online::PartyJoinCallback m_pfnCallback;  // 0x4b8
};
static_assert(sizeof(PartyPendingJoinInfo) == 0x4c0, "PartyPendingJoinInfo");

typedef bdReference<Console::Online::Party> PartyRef;

class Party : public bdReferencable, public bdConnectionListener, public bdDispatchInterceptor {  // size 0x578
public:
    BD_CLASS_ALLOCATOR(Party);  // line 334
    Party(int32);
    virtual ~Party();  // vtable slot 0
    Console::Online::PartySwitchOnlineResult CheckOnlineStateEligibility(Console::Online::PartyOnlineState eState);
    void SetOnlineState(Console::Online::PartyOnlineState eState);
    void FinalizeOnlineStateChange();
    Console::Online::PartyOnlineState GetOnlineState() const { return m_eOnlineState; }  // line 344
    void SetPreferredOnlineState(Console::Online::PartyOnlineState eState);
    Console::Online::PartyOnlineState GetPreferredOnlineState() const { return m_ePreferredOnlineState; }  // line 348
    BOOL IsOffline() const { return m_eOnlineState == PARTY_OFFLINE; }  // line 352
    BOOL IsLAN() const { return m_eOnlineState == PARTY_LAN; }  // line 353
    BOOL IsAdHOC() const { return m_eOnlineState == PARTY_ADHOC; }  // line 355
    BOOL IsOnline() const { return m_eOnlineState >= PARTY_ONLINE; }  // line 357
    BOOL IsPublic() const { return m_eOnlineState == PARTY_ONLINE_PUBLIC; }  // line 358
    BOOL IsHost() const { return m_bIsHost; }  // line 359
    BOOL CanStartGame() const;
    BOOL IsGameStarted() const { return m_bGameInProgress; }  // line 362
    BOOL IsInGame() const { return m_bInGame; }  // line 363
    BOOL IsConnected() const;
    BOOL IsMigrating() const { return m_eMigrationState != MIGRATION_IDLE; }  // line 367
    int32 GetControllerIndex() const { return m_nControllerIndex; }  // line 369
    enum ConnectionState {
        PARTY_NOT_CONNECTED = 0,
        PARTY_CONNECTING = 1,
        PARTY_NEGOTIATING = 2,
        PARTY_CONNECTED = 3,
        PARTY_CONNECTION_FAILED = 4,
        PARTY_SHUTTING_DOWN = 5,
    };
    Console::Online::Party::ConnectionState GetConnectionState() const { return m_eConnectionState; }  // line 385
    void Idle();
    void Cleanup();
    void Create(const Console::Online::GameParams & tGameParams);
    void Join(Console::Online::PartyPendingJoinInfo & tJoinInfo);
    void Leave();
    void OnSuspend();
    void OnResume();
    int32 FindPlayer(bdCommonAddrRef ptAddr, int32 nControllerIndex);
    int32 FindLocalPlayer(int32 nControllerIndex);
    Console::Online::PartyPlayer * AddPlayer(bdCommonAddrRef ptAddr, int32 nControllerIndex, const char * szPlayerName, const Console::Online::AdditionalPlatformInformation & tAdditionalPlatformInfo, OnlineUserId idOnlineUser, Console::Online::PlayerType ePlayerType, int32 nSponsorIndex, BOOL bHasVoicePrivs, BOOL bBadReputation);
    Console::Online::PartyPlayer * AddPlayer(Console::Online::PartyPlayer & tPlayer);
    Console::Online::PartyPlayer * AddPlayer(bdCommonAddrRef ptAddr, Console::Online::D3PartyMsgAddPlayerRequest & tRequest);
    BOOL RemovePlayer(bdCommonAddrRef ptAddr, int32 nControllerIndex);
    BOOL RemovePlayer(int32 nPlayerIndex);
    int32 GetFirstPlayerIndex() const;
    int32 GetNextPlayerIndex(int32 nPlayerIndex) const;
    int32 GetFirstLocalPlayerIndex() const;
    int32 GetNextLocalPlayerIndex(int32 nPlayerIndex) const;
    void SetHostPlayer(bdByte8 idHostPlayer);
    void SendPartyInfo(bdConnectionRef ptConnection) const;
    enum SendAccountMode {
        SEND_ACCOUNT_AND_HERO = 0,
        SEND_HERO_ONLY = 1,
    };
    void SendPlayerAccountAndHeroToHost(Console::Online::PartyPlayer & tPlayer, Console::Online::Party::SendAccountMode eMode) const;
    void SendHeroDigest(uint8 idPlayer, bdConnectionRef ptConnection, bdConnectionRef ptSkipConnection) const;
    void StartGameForPeer(bdConnectionRef ptConnection);
    BOOL StartGame();
    void ServerEndingGame();
    void EndGame();
    bdConnectionRef InternalConnect(bdCommonAddrRef ptAddress, bdSecurityID tSecurityID);
    void CloseConnection(bdConnectionRef ptConnection);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    Console::Online::JoinResult onConnectRequest(Console::Online::D3PartyJoinRequest & tJoinRequest, Console::Online::D3PartyJoinResponse & tJoinResponse);
    void onCreateSession(BOOL, int32 &) {}  // line 523 (empty in-class body: no concrete DIE; call_callback<..onCreateSession> is a bare ret)
    void onJoinSession(BOOL, int32 &) {}  // line 527 (empty in-class body: no concrete DIE; call_callback<..onJoinSession> is a bare ret)
    virtual void onConnect(bdConnectionRef ptConnection);  // vtable slot 2
    virtual void onConnectFailed(bdConnectionRef ptConnection);  // vtable slot 3
    virtual void onDisconnect(bdConnectionRef ptConnection);  // vtable slot 4
    virtual void onReconnect(bdConnectionRef connection) { CloseConnection(connection); }  // line 558  // vtable slot 5
    BOOL onPartyJoined(const bdBitBufferRef ptBuffer);
    void onPartyJoinResult(Console::Online::JoinResult eResult);
    virtual void onPeerConnect(const bdConnectionRef ptConnection);  // vtable slot 6
    virtual void onPeerDisconnect(const bdConnectionRef ptConnection);  // vtable slot 7
    virtual bdBool accept(const bdReceivedMessage & tReceivedMsg);  // vtable slot 8
    virtual bdBool acceptOutOfOrder(const bdReceivedMessage & tReceivedMsg);  // vtable slot 9
    Console::Online::PacketResult HandleJoinRequest(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleJoinResponse(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandlePeerHello(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleAddPlayerRequest(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleAddPlayerResponse(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleRemovePlayerRequest(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleUpdatePlayersRequest(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult HandleStartGame();
    Console::Online::PacketResult HandleIAmHost(const bdReceivedMessage & tReceivedMsg);
    Console::Online::PacketResult ReceiveChunkedData(const bdConnectionRef ptConnection, const bdMessageRef ptMessage, Console::Online::PartyChunkedDataType & eChunkType, Console::Online::PartyPlayer *& ptPlayer, BOOL & bComplete);
    void SendChunkedData(bdByte8 idPlayer, Console::Online::PartyChunkedDataType eType, const google::protobuf::Message & tMessage, bdConnectionRef ptConnection, bdConnectionRef ptSkipConnection) const;
    void SendChallengeRift(bdConnectionRef ptConnection) const;
    BOOL SendToHost(bdMessageRef ptMessage) const;
    BOOL SendTo(bdConnectionRef ptConnection, bdMessageRef ptMessage) const;
    BOOL Send(bdMessageRef ptMessage) const;
    void GetGameInfo(Console::Online::GameInfo & tGameInfo);
    void UpdateLAN(const Console::Online::GameInfo & tGameInfo);
    void UpdateSession();
    void UpdatePlayHistory();
    void UpdateVoiceChatSession(BOOL bIsOnPartyJoined);
    enum MigrationState {
        MIGRATION_IDLE = 0,
        MIGRATION_VALIDATING = 1,
        MIGRATION_ACTIVE = 2,
        MIGRATION_BECOMING_HOST = 3,
        MIGRATION_FINALIZING = 4,
        MIGRATION_DISABLED = 5,
    };
    void SetMigrationState(Console::Online::Party::MigrationState eState);
    BOOL HaveAllPeersRequestedMigration();
    void UpdateMigrationInfo();
    void StartMigrating(bdCommonAddrRef ptHostAddress);
    void FailMigration();
    void FinalizeMigration();
    void BecomeHost();
    void ForceMigrate();
    void GetMatchParameters(Console::Online::MatchParameters & tParams) const;
    Console::Online::Party::ConnectionState m_eConnectionState;  // 0x20
    bdConnectionRef m_ptHostConnection;  // 0x28
    blz::vector<bdReference<Console::Online::Peer>, blz::allocator<bdReference<Console::Online::Peer> > > m_aptPeers;  // 0x30
    Console::Online::PeerRef AddPeer(bdConnectionRef ptConnection, bdNATType eNATType);
    Console::Online::PeerRef FindPeer(bdCommonAddrRef ptAddress) const;
    Console::Online::PeerRef FindPeer(bdConnectionRef ptConnection) const;
    BOOL IsSameParty(Console::Online::PartyConnectionInfo & tConnection);
    bdMessageRef m_ptJoinRequestMessage;  // 0x48
    Console::Online::PartyJoinCallback m_pfnJoinCallback;  // 0x50
    typedef OnlineService::Callback<void ()> OnlineStateChangeCallback;
    Console::Online::Party::OnlineStateChangeCallback m_pfnOnlineStateChangeCallback;  // 0x58
    Console::NX64::SessionManager * m_ptSessionManager;  // 0x60
    bdString m_idSession;  // 0x68
    BOOL m_bOwnsPresenceSessionKey;  // 0x70
    bdSecurityID m_tSecID;  // 0x74
    bdSecurityKey m_tSecKey;  // 0x7c
    BOOL m_bRegisteredKey;  // 0x8c
    BOOL m_bIsHost;  // 0x90
    bdByte8 m_idHostPlayer;  // 0x94
    BOOL m_bGameInfoDirty;  // 0x98
    Blizzard::Time::Millisecond m_uLastGameInfoUpdateTime;  // 0x9c
    void MarkGameInfoDirty() { m_bGameInfoDirty = TRUE; }  // line 750
    BOOL m_bGameInProgress;  // 0xa0
    BOOL m_bInGame;  // 0xa4
    BOOL m_bIsPaused;  // 0xa8
    BOOL m_bIsSoftPaused;  // 0xac
    BOOL m_bMatchmakingEnabled;  // 0xb0
    Console::Online::PartyOnlineState m_eOnlineState;  // 0xb4
    Console::Online::PartyOnlineState m_ePendingOnlineState;  // 0xb8
    Console::Online::PartyOnlineState m_ePreferredOnlineState;  // 0xbc
    BOOL IsOnlineStateChanging() const;
    int32 m_nControllerIndex;  // 0xc0
    Console::Online::GameParams m_tGameParams;  // 0xc8
    uint8 m_uMaxPlayers;  // 0x188
    uint8 m_uNumPlayers;  // 0x189
    Console::Online::PartyPlayer m_tPlayers[4];  // 0x190
    uint32 m_fPlayersInGame;  // 0x510
    Console::Online::Party::MigrationState m_eMigrationState;  // 0x514
    Blizzard::Time::Millisecond m_uMigrationStateStart;  // 0x518
    blzext::fixed_vector<bdReference<bdCommonAddr>, 4> m_aptMigrationInfo;  // 0x520
    BOOL m_bQoSListening;  // 0x550
    Console::Online::PartyAddPlayerCallback m_apfnAddPlayerCallbacks[4];  // 0x558
};
static_assert(sizeof(Party) == 0x578, "Party");

struct D3PartyJoinRequest {  // size 0x3a0
    D3PartyJoinRequest(bdBool, bdNATType, Console::Online::JoinType);
    Console::Online::PartyPlayer & AddPlayer(Console::Online::PartyPlayer & tAddPlayer);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdUInt64 m_uTitleVersion;  // 0x00
    bdUByte8 m_uNumPlayers;  // 0x08
    Console::Online::PartyPlayer m_tPlayers[4];  // 0x10
    bdUInt32 m_uNATType;  // 0x390
    bdUInt32 m_eJoinType;  // 0x394
    bdBool m_bIsOnline;  // 0x398
};
static_assert(sizeof(D3PartyJoinRequest) == 0x3a0, "D3PartyJoinRequest");

struct D3PartyJoinResponse {  // size 0x58
    D3PartyJoinResponse(bdUInt32);
    void AddPeers(const blz::vector<bdReference<Console::Online::Peer>, blz::allocator<bdReference<Console::Online::Peer> > > & aptPeers);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    struct Peer {  // size 0x10
        bdCommonAddrRef ptAddr;  // 0x00
        bdUInt32 uNATType;  // 0x08
    };
    bdUInt32 m_uResult;  // 0x00
    blzext::fixed_vector<Console::Online::D3PartyJoinResponse::Peer, 4> m_atPeers;  // 0x08
};
static_assert(sizeof(D3PartyJoinResponse) == 0x58, "D3PartyJoinResponse");

struct D3PartyMsgPeerHello {  // size 0x1c
    D3PartyMsgPeerHello(bdSecurityID, bdSecurityKey, bdNATType);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdSecurityID m_tSecID;  // 0x00
    bdSecurityKey m_tSecKey;  // 0x08
    bdUInt32 m_uNATType;  // 0x18
};
static_assert(sizeof(D3PartyMsgPeerHello) == 0x1c, "D3PartyMsgPeerHello");

struct D3PartyMsgJoinParty {  // size 0x20
    D3PartyMsgJoinParty(bdCommonAddrRef, bdSecurityID, bdSecurityKey);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdCommonAddrRef m_ptAddr;  // 0x00
    bdSecurityID m_tSecID;  // 0x08
    bdSecurityKey m_tSecKey;  // 0x10
};
static_assert(sizeof(D3PartyMsgJoinParty) == 0x20, "D3PartyMsgJoinParty");

struct D3PartyMsgAddPlayerRequest {  // size 0x80
    D3PartyMsgAddPlayerRequest(int32, const char *, Console::Online::AdditionalPlatformInformation, OnlineUserId, Console::Online::PlayerType, int32, BOOL, BOOL);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdInt32 m_nControllerIndex;  // 0x00
    char m_szPlayerName[32];  // 0x04
    Console::Online::AdditionalPlatformInformation m_tAdditionalPlatformInfo;  // 0x24
    OnlineUserId m_idOnlineUser;  // 0x68
    bdUInt32 m_ePlayerType;  // 0x70
    bdInt32 m_nGuestSponsor;  // 0x74
    bdBool m_bVoiceActive;  // 0x78
    bdBool m_bBadReputation;  // 0x79
};
static_assert(sizeof(D3PartyMsgAddPlayerRequest) == 0x80, "D3PartyMsgAddPlayerRequest");

struct D3PartyMsgAddPlayerResponse {  // size 0x8
    D3PartyMsgAddPlayerResponse(bdInt32, bdByte8);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdInt32 m_nControllerIndex;  // 0x00
    bdByte8 m_idPlayer;  // 0x04
};
static_assert(sizeof(D3PartyMsgAddPlayerResponse) == 0x8, "D3PartyMsgAddPlayerResponse");

struct D3PartyMsgRemovePlayer {  // size 0x4
    D3PartyMsgRemovePlayer(int32);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdInt32 m_nControllerIndex;  // 0x00
};
static_assert(sizeof(D3PartyMsgRemovePlayer) == 0x4, "D3PartyMsgRemovePlayer");

struct D3PartyMsgPlayerDataChunk {  // size 0xc
    D3PartyMsgPlayerDataChunk(bdUByte8, Console::Online::PartyChunkedDataType, uint32);
    uint32 Serialize(bdBitBufferRef ptBitBuffer, const uint8 * ptData, uint32 uDataSize) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer, uint8 * ptData, uint32 & uDataSize);
    bdUByte8 m_idPlayer;  // 0x00
    bdBool m_bComplete;  // 0x01
    Console::Online::PartyChunkedDataType m_eType;  // 0x04
    bdUInt32 m_uUncompressedSize;  // 0x08
};
static_assert(sizeof(D3PartyMsgPlayerDataChunk) == 0xc, "D3PartyMsgPlayerDataChunk");

struct D3PartyMsgMigrationRequest {  // size 0x8
    D3PartyMsgMigrationRequest();
    void Serialize(bdBitBufferRef ptBitBuffer);
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdCommonAddrRef m_ptHostAddress;  // 0x00
};
static_assert(sizeof(D3PartyMsgMigrationRequest) == 0x8, "D3PartyMsgMigrationRequest");

struct D3PartyMsgMigrationIAmHost {  // size 0x50
    D3PartyMsgMigrationIAmHost();
    void AddPeers(const blz::vector<bdReference<Console::Online::Peer>, blz::allocator<bdReference<Console::Online::Peer> > > & aptPeers);
    void AddSelf(const bdCommonAddrRef ptAddress, bdNATType eNATType);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    struct Peer {  // size 0x10
        bdCommonAddrRef ptAddr;  // 0x00
        bdUInt32 uNATType;  // 0x08
    };
    blzext::fixed_vector<Console::Online::D3PartyMsgMigrationIAmHost::Peer, 4> m_atPeers;  // 0x00
};
static_assert(sizeof(D3PartyMsgMigrationIAmHost) == 0x50, "D3PartyMsgMigrationIAmHost");

struct D3PartyMsgUpdatePlayers {  // size 0xe8
    D3PartyMsgUpdatePlayers();
    void AddPlayer(int32, const char *, OnlineUserId, BOOL);
    void Serialize(bdBitBufferRef) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    struct PlayerInfo {  // size 0x38
        bdInt32 m_nControllerIndex;  // 0x00
        char m_szPlayerName[32];  // 0x04
        OnlineUserId m_idOnlineUser;  // 0x28
        bdBool m_bBadReputation;  // 0x30
    };
    bdUByte8 m_uNumPlayers;  // 0x00
    Console::Online::D3PartyMsgUpdatePlayers::PlayerInfo m_tPlayerInfo[4];  // 0x08
};
static_assert(sizeof(D3PartyMsgUpdatePlayers) == 0xe8, "D3PartyMsgUpdatePlayers");

struct D3PartyMsgPlayerInGame {  // size 0x2
    D3PartyMsgPlayerInGame(uint8, BOOL);
    void Serialize(bdBitBufferRef ptBitBuffer) const;
    bdBool Deserialize(bdBitBufferRef ptBitBuffer);
    bdUByte8 m_idPlayer;  // 0x00
    bdBool m_bInGame;  // 0x01
};
static_assert(sizeof(D3PartyMsgPlayerInGame) == 0x2, "D3PartyMsgPlayerInGame");

typedef blz::list<bdReference<Console::Online::Party>, blz::allocator<bdReference<Console::Online::Party> > > PartyList;

}  // namespace Online
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line   157  BOOL Console::Online::PartyPlayer::IsReserved()  [inlined into 5 functions]
//   line   179  BOOL Console::Online::PartyPlayer::IsValid() const  [inlined into 39 functions]
//   line   204  BOOL Console::Online::PartyPlayer::IsPrimary() const  [inlined into 5 functions]
//   line   210  BOOL Console::Online::PartyPlayer::IsGuest() const  [inlined into 2 functions]
//   line   216  BOOL Console::Online::PartyPlayer::IsInGame() const  [inlined into 1 functions]
//   line   267  Console::Online::Peer::~Peer()  [0x710035f140 (+1 more), inlined into 1 functions]
//   line   270  void Console::Online::Peer::operator delete(void * p)  [inlined into 1 functions]
//   line   272  Console::Online::Peer::Peer(bdConnectionRef ptConnection, bdNATType eNATType)  [inlined into 5 functions]
//   line   296  Console::Online::PartyPendingJoinInfo::~PartyPendingJoinInfo()  [inlined into 3 functions]
//   line   306  Console::Online::PartyPendingJoinInfo::PartyPendingJoinInfo(Console::Online::PartyConnectionInfo & tConnectionInfo, int32 nControllerIndex, Console::Online::JoinType eJoinType, Console::Online::PartyJoinCallback pfnCallback)  [inlined into 3 functions]
//   line   334  void Console::Online::Party::operator delete(void * p)  [inlined into 1 functions]
//   line   344  Console::Online::PartyOnlineState Console::Online::Party::GetOnlineState() const  [inlined into 6 functions]
//   line   348  Console::Online::PartyOnlineState Console::Online::Party::GetPreferredOnlineState() const  [inlined into 1 functions]
//   line   352  BOOL Console::Online::Party::IsOffline() const  [inlined into 2 functions]
//   line   353  BOOL Console::Online::Party::IsLAN() const  [inlined into 3 functions]
//   line   355  BOOL Console::Online::Party::IsAdHOC() const  [inlined into 2 functions]
//   line   357  BOOL Console::Online::Party::IsOnline() const  [inlined into 15 functions]
//   line   358  BOOL Console::Online::Party::IsPublic() const  [inlined into 2 functions]
//   line   359  BOOL Console::Online::Party::IsHost() const  [inlined into 35 functions]
//   line   361  BOOL Console::Online::Party::CanStartGame() const  [inlined into 2 functions]
//   line   362  BOOL Console::Online::Party::IsGameStarted() const  [inlined into 5 functions]
//   line   363  BOOL Console::Online::Party::IsInGame() const  [inlined into 18 functions]
//   line   365  BOOL Console::Online::Party::IsConnected() const  [inline]
//   line   367  BOOL Console::Online::Party::IsMigrating() const  [inlined into 1 functions]
//   line   369  int32 Console::Online::Party::GetControllerIndex() const  [inlined into 5 functions]
//   line   385  Console::Online::Party::ConnectionState Console::Online::Party::GetConnectionState() const  [inlined into 2 functions]
//   line   558  void Console::Online::Party::onReconnect(bdConnectionRef connection)  [0x710035f020]
//   line   750  void Console::Online::Party::MarkGameInfoDirty()  [inlined into 14 functions]
//   line   766  BOOL Console::Online::Party::IsOnlineStateChanging() const  [inlined into 2 functions]
//   line   792  Console::Online::D3PartyJoinRequest::~D3PartyJoinRequest()  [0x71003542e0]
//   line   819  Console::Online::D3PartyJoinResponse::~D3PartyJoinResponse()  [inlined into 2 functions]
//   line   833  Console::Online::D3PartyJoinResponse::Peer::Peer()  [inlined into 2 functions]
//   line   833  Console::Online::D3PartyJoinResponse::Peer::~Peer()  [inlined into 2 functions]
//   line   845  Console::Online::D3PartyMsgPeerHello::~D3PartyMsgPeerHello()  [inlined into 2 functions]
//   line   866  Console::Online::D3PartyMsgJoinParty::~D3PartyMsgJoinParty()  [inlined into 2 functions]
//   line   980  Console::Online::D3PartyMsgMigrationRequest::~D3PartyMsgMigrationRequest()  [inlined into 2 functions]
//   line   999  Console::Online::D3PartyMsgMigrationIAmHost::~D3PartyMsgMigrationIAmHost()  [inlined into 2 functions]
//   line  1016  Console::Online::D3PartyMsgMigrationIAmHost::Peer::Peer()  [inlined into 3 functions]
//   line  1016  Console::Online::D3PartyMsgMigrationIAmHost::Peer::~Peer()  [inlined into 2 functions]

// ConsoleParty.h:361 and :766; written from the copies inlined into X/Client/Console/Online.cpp.
namespace Console {
namespace Online {
inline BOOL Party::CanStartGame() const { return m_bIsHost || m_bGameInProgress; }
inline BOOL Party::IsOnlineStateChanging() const { return m_eOnlineState != m_ePendingOnlineState; }
// ConsoleParty.h:306.
inline PartyPendingJoinInfo::PartyPendingJoinInfo(PartyConnectionInfo & tConnectionInfo, int32 nControllerIndex, JoinType eJoinType, PartyJoinCallback pfnCallback)
    : PartyConnectionInfo(tConnectionInfo), m_nControllerIndex(nControllerIndex), m_eJoinType(eJoinType), m_pfnCallback(pfnCallback)
{
}
}  // namespace Online
}  // namespace Console
