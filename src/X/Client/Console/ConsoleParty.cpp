#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "ConsoleParty.h"
#include <string.h>
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdNet.h"
#include "../../Common/Console/Networking/Connection.h"
#include "GamerProfile.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
// Console::NX64::SessionManager is not described by the DWARF; only its virtual calls are
// visible. The method names are invented.
namespace Console { namespace NX64 {
class SessionManager {
public:
    virtual void Destroy() = 0;  // vtable slot 0
    virtual void CreateSession(const Console::Online::PartyConnectionInfo & tConnectionInfo, Console::Online::PartyOnlineState eOnlineState, const int32 * anControllerIndices, int32 nNumControllers, const OnlineService::Callback<void (int, int &)> & pfnCallback) = 0;  // vtable slot 1 (invented name/params; Finalize cpp847 calls +0x8 with x1..x5)
    virtual void JoinSession(const char * szSessionId, const int32 * anControllerIndices, int32 nNumControllers, const OnlineService::Callback<void (int, int &)> & pfnCallback) = 0;  // vtable slot 2 (invented name/params; onPartyJoined cpp2609 calls +0x10 with x1..x4 = id buffer, users, count, callback)
    virtual void LeaveSession() = 0;  // vtable slot 3 (invented name; Leave cpp1396 calls +0x18 with no args)
    virtual void Unknown4() = 0;  // vtable slot 4
    virtual void Unknown5() = 0;  // vtable slot 5
    virtual void SetLocalUsers(const int32 * anControllerIndices, uint32 uNumControllers) = 0;  // vtable slot 6
    virtual void SetOnlineState(Console::Online::PartyOnlineState eState) = 0;  // vtable slot 7
    virtual void Unknown8() = 0;  // vtable slot 8
    virtual BOOL IsBusy() = 0;  // vtable slot 9 (invented name; Idle cpp950 calls +0x48 with no args and tests the result)
};
} }
namespace Console {
namespace Online {

PartyRef sptParty;
PartyList sgtParties;

volatile int32 snGameTag;

PartyPlayer::PartyPlayer()
    : m_idPlayer(-1),
      m_ptAddr(NULL),
      m_nControllerIndex(-1),
      m_ePlayerType(PLAYER_REGULAR),
      m_nSponsorIndex(-1),
      m_bVoiceActive(false),
      m_bBadReputation(false),
      m_uReservationTime(0),
      m_bInGame(false),
      m_bIsLeaving(FALSE),
      m_ptAccountDefinition(NULL),
      m_ptHeroDefinition(NULL),
      m_ptHeroDigest(NULL),
      m_uChunkUncompressedSize(0),
      m_eChunkType(CHUNKED_DATA_INVALID)
{
    SigmaMemoryClear(m_szName, sizeof(m_szName));
    SigmaMemoryClear(&m_idOnlineUser, sizeof(m_idOnlineUser));
}

inline PartyPlayer::~PartyPlayer()
{
    Clear();
}

void PartyPlayer::Set(uint8 idPlayer, bdCommonAddrRef ptAddr, int32 nControllerIndex, const char * szName, const AdditionalPlatformInformation & tAdditionalPlatformInfo, OnlineUserId & idOnlineUser, PlayerType ePlayerType, int32 nSponsorIndex, BOOL bVoiceActive, BOOL bBadReputation)
{
    m_idPlayer = idPlayer;
    m_ptAddr = ptAddr;
    m_nControllerIndex = nControllerIndex;
    Blizzard::String::Copy(m_szName, szName, sizeof(m_szName));
    SigmaMemoryCopy(&m_tAdditionalPlatformInfo, &tAdditionalPlatformInfo, sizeof(m_tAdditionalPlatformInfo));
    m_idOnlineUser = idOnlineUser;
    m_ePlayerType = ePlayerType;
    m_nSponsorIndex = nSponsorIndex;
    m_bVoiceActive = bVoiceActive != FALSE;
    m_bBadReputation = bBadReputation != FALSE;
}

void PartyPlayer::Clear()
{
    m_idPlayer = -1;
    m_ptAddr = NULL;
    m_nControllerIndex = -1;
    m_ePlayerType = PLAYER_REGULAR;
    m_nSponsorIndex = -1;
    m_bVoiceActive = false;
    m_bInGame = false;
    m_bIsLeaving = FALSE;
    SigmaMemoryClear(&m_tAdditionalPlatformInfo, sizeof(m_tAdditionalPlatformInfo));

    ::delete m_ptAccountDefinition;
    m_ptAccountDefinition = NULL;
    ::delete m_ptHeroDefinition;
    m_ptHeroDefinition = NULL;
    ::delete m_ptHeroDigest;
    m_ptHeroDigest = NULL;

    ReleaseChunkedData();
}

inline void PartyPlayer::ReleaseChunkedData()
{
    blz::string().swap(m_szChunkDataBuffer);
    m_uChunkUncompressedSize = 0;
}

BOOL PartyPlayer::IsLocal() const
{
    return m_ptAddr.isNull() || m_ptAddr->isLoopback();
}

BOOL PartyPlayer::IsLocal(int32 nControllerIndex) const
{
    return IsLocal() && m_nControllerIndex == nControllerIndex;
}

BOOL PartyPlayer::IsAddressAndIndex(const bdCommonAddrRef ptAddr, int32 nControllerIndex) const
{
    return IsSameAddress(m_ptAddr, ptAddr) && m_nControllerIndex == nControllerIndex;
}

BOOL PartyPlayer::IsSamePlayer(const PartyPlayer & tPlayer) const
{
    return IsAddressAndIndex(tPlayer.m_ptAddr, tPlayer.m_nControllerIndex);
}

const D3::Account::SavedDefinition * PartyPlayer::GetAccount() const
{
    return m_ptAccountDefinition;
}

const D3::Hero::SavedDefinition * PartyPlayer::GetHero() const
{
    if (IsLocal() && !GamerProfile::IsHeroLoaded(m_nControllerIndex))
        return NULL;
    return m_ptHeroDefinition;
}

Party::~Party()
{
    Cleanup();

    {
        bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
        ptNet->getConnectionStore()->unregisterDispatchInterceptor(this);

        if (m_bRegisteredKey && ptNet->getSocketRouter() != NULL)
            ptNet->getSocketRouter()->getKeyMap()->unregisterKey(m_tSecID);
    }

    if (m_ptSessionManager != NULL) {
        m_ptSessionManager->Destroy();
    }
}

void Party::Cleanup()
{
    if (m_ptHostConnection.notNull())
        CloseConnection(m_ptHostConnection);

    for (PeerRef ptPeer : m_aptPeers)
        CloseConnection(ptPeer->m_ptConnection);

    m_ptJoinRequestMessage = NULL;
    m_pfnJoinCallback.Clear();

    bdSingleton<bdNetImpl>::getInstance()->getConnectionStore()->unregisterDispatchInterceptor(this);
}

void Party::Create(const GameParams & tGameParams)
{
    bdCommonAddrRef ptLocalAddr = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    m_bIsHost = TRUE;
    m_tGameParams = tGameParams;
    m_eConnectionState = PARTY_CONNECTED;
    MarkGameInfoDirty();
    UpdateVoiceChatSession(TRUE);
    QueueNotification(GetPartyJoinedEvent());
}

void Party::SetPreferredOnlineState(PartyOnlineState eState)
{
    if (eState > PARTY_LAN)
        m_ePreferredOnlineState = eState;
}


#line 191
void PartyPlayer::ClearReservation()

{
    m_uReservationTime = 0;
}
#line 191



BOOL Party::RemovePlayer(int32 nPlayerIndex)
{
    PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
    if (!tPlayer.IsValid())
        return FALSE;

    tPlayer.Clear();
    --m_uNumPlayers;
    MarkGameInfoDirty();
    QueueNotification(GetPartyMemberRemovedEvent(), nPlayerIndex);
    return TRUE;
}

int32 Party::GetFirstPlayerIndex() const
{
    return GetNextPlayerIndex(-1);
}



// DWARF: GetNextPlayerIndex (cpp:1640) has no local; it steps its parameter (row 1645:39 is the `<`)
int32 Party::GetNextPlayerIndex(int32 nPlayerIndex) const
{
    for (++nPlayerIndex; nPlayerIndex < MAX_PEERS; ++nPlayerIndex) {
        if (m_tPlayers[nPlayerIndex].IsValid())
            return nPlayerIndex;
    }
    return -1;
}


int32 Party::GetFirstLocalPlayerIndex() const
{
    return GetNextLocalPlayerIndex(-1);
}

int32 Party::GetNextLocalPlayerIndex(int32 nPlayerIndex) const
{
    for (int32 i = GetNextPlayerIndex(nPlayerIndex); i != -1; i = GetNextPlayerIndex(i)) {
        if (m_tPlayers[i].IsLocal())
            return i;
    }
    return -1;
}

int32 Party::FindPlayer(bdCommonAddrRef ptAddr, int32 nControllerIndex)
{
    for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i)) {
        if (m_tPlayers[i].IsAddressAndIndex(ptAddr, nControllerIndex))
            return i;
    }
    return -1;
}

int32 Party::FindLocalPlayer(int32 nControllerIndex)
{
    for (int32 i = GetFirstLocalPlayerIndex(); i != -1; i = GetNextLocalPlayerIndex(i)) {
        if (m_tPlayers[i].m_nControllerIndex == nControllerIndex)
            return i;
    }
    return -1;
}

void Party::SetHostPlayer(bdByte8 idHostPlayer)
{
    m_idHostPlayer = idHostPlayer;
}

void Party::ServerEndingGame()
{
    if (IsHost()) {
        GameClearServerPendingConnections();
        GameMarkServerGameConnectionsDontClose();
        m_bIsPaused = FALSE;
        m_bIsSoftPaused = FALSE;
    }
}

bdBool Party::acceptOutOfOrder(const bdReceivedMessage & tReceivedMsg)
{
    return accept(tReceivedMsg);
}











void Party::SendChallengeRift(bdConnectionRef ptConnection) const
{
    SendChunkedData(m_idHostPlayer, CHUNKED_DATA_CHALLENGE_RIFT, m_tGameParams.m_tWeeklyChallengeData, ptConnection, NULL);
}

void Party::UpdateSession()
{
    int32 anControllerIndices[MAX_PEERS];
    uint32 uNumControllers = 0;
    for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i)) {
        PartyPlayer & tPlayer = m_tPlayers[i];
        if (!tPlayer.IsReserved() && tPlayer.IsLocal() && !tPlayer.m_bIsLeaving)
            anControllerIndices[uNumControllers++] = tPlayer.m_nControllerIndex;
    }
    if (m_ptSessionManager != NULL) {
        m_ptSessionManager->SetLocalUsers(anControllerIndices, uNumControllers);
        m_ptSessionManager->SetOnlineState(m_eOnlineState);
    }
}

void Party::CloseConnection(bdConnectionRef ptConnection)
{
    GameNetwork::ConnectionRef ptGameConnection = GameGetGameConnection(ptConnection);

    ptConnection->unregisterListener(this);
    ptConnection->disconnect();

    if (ptGameConnection)
        ptGameConnection->onDisconnect();
}

BOOL Party::SendToHost(bdMessageRef ptMessage) const
{
    if (m_ptHostConnection.isNull())
        return FALSE;
    ScopedNetworkSendLock tLock;
    return m_ptHostConnection->send(ptMessage, true);
}

BOOL Party::Send(bdMessageRef ptMessage) const
{
    ScopedNetworkSendLock tLock;
    BOOL bResult = TRUE;
    for (PeerRef ptPeer : m_aptPeers) {
        bdConnectionRef ptConnection = ptPeer->m_ptConnection;
        bResult = ptConnection->send(ptMessage, true) && bResult;
    }
    return bResult;
}

PeerRef Party::FindPeer(bdCommonAddrRef ptAddress) const
{
    for (PeerRef ptPeer : m_aptPeers) {
        if (IsSameAddress(ptPeer->m_ptConnection->getAddress(), ptAddress))
            return ptPeer;
    }
    return NULL;
}

BOOL Party::IsSameParty(PartyConnectionInfo & tConnection)
{
    BOOL bSameAddress;
    if (m_ptHostConnection.notNull())
        bSameAddress = IsSameAddress(m_ptHostConnection->getAddress(), tConnection.m_ptAddress);
    else
        bSameAddress = tConnection.m_ptAddress->isLoopback();
    if (!bSameAddress)
        return FALSE;
    return memcmp(&m_tSecID, &tConnection.m_tSecID, sizeof(m_tSecID)) == 0 && memcmp(&m_tSecKey, &tConnection.m_tSecKey, sizeof(m_tSecID)) == 0;
}















void Party::SetMigrationState(MigrationState eState)
{
    if (IsInGame()) {
        if (eState > MIGRATION_VALIDATING) {
            if (m_eMigrationState <= MIGRATION_VALIDATING)
                QueueNotification(GetPartyMigratingEvent(), MIGRATION_STARTED);
        } else if (eState == MIGRATION_IDLE) {
            if (m_eMigrationState > MIGRATION_VALIDATING)
                QueueNotification(GetPartyMigratingEvent(), MIGRATION_FINISHED);
        }
    }

    m_eMigrationState = eState;
    m_uMigrationStateStart = eState == MIGRATION_IDLE ? 0 : Blizzard::Time::Milliseconds();
}

void Party::ForceMigrate()
{
    if (m_ptHostConnection.notNull()) {
        for (PeerRef ptPeer : m_aptPeers)
            CloseConnection(ptPeer->m_ptConnection);
        m_aptPeers.clear();

        onDisconnect(m_ptHostConnection);

        if (m_eMigrationState != MIGRATION_DISABLED)
            SetMigrationState(MIGRATION_BECOMING_HOST);
    }
}

}  // namespace Online
}  // namespace Console














































namespace Console { namespace Online {
#line 210
void PartyPlayer::Serialize(bdBitBufferRef ptBitBuffer) const
{

    m_ptAddr->serialize(ptBitBuffer);
    ptBitBuffer->writeRangedInt32(m_nControllerIndex, 0, MAX_PEERS - 1);
    ptBitBuffer->writeRangedUInt32(m_ePlayerType, 0, NUM_PLAYER_TYPES, true);
    ptBitBuffer->writeRangedInt32(m_nSponsorIndex, -1, MAX_PEERS - 1);
    ptBitBuffer->writeBool(m_bVoiceActive);
    ptBitBuffer->writeString(bdString(m_szName));


    ptBitBuffer->writeFull(m_tAdditionalPlatformInfo);


    ptBitBuffer->writeFull(m_idOnlineUser);
    ptBitBuffer->writeBool(m_bInGame);
    ptBitBuffer->writeBool(m_bBadReputation);
}
} }
#line 472







namespace Console { namespace Online {
#line 231

bdBool PartyPlayer::Deserialize(
    bdCommonAddrRef me,
    bdBitBufferRef ptBitBuffer)
{
    bdUInt32 ePlayerType = PLAYER_REGULAR;

    m_ptAddr = ::new bdCommonAddr();
    bdBool bOk = m_ptAddr->deserialize(me, ptBitBuffer);
    bOk = bOk && ptBitBuffer->readRangedInt32(m_nControllerIndex, 0, MAX_PEERS - 1);
    bOk = bOk && ptBitBuffer->readRangedUInt32(ePlayerType, 0, NUM_PLAYER_TYPES, true);
    bOk = bOk && ptBitBuffer->readRangedInt32(m_nSponsorIndex, -1, MAX_PEERS - 1);
    bOk = bOk && ptBitBuffer->readBool(m_bVoiceActive);
    bOk = bOk && ptBitBuffer->readString(m_szName, sizeof(m_szName));


    bOk = bOk && ptBitBuffer->readFull(m_tAdditionalPlatformInfo);


    bOk = bOk && ptBitBuffer->readFull(m_idOnlineUser);
    bOk = bOk && ptBitBuffer->readBool(m_bInGame);
    bOk = bOk && ptBitBuffer->readBool(m_bBadReputation);
    if (!bOk)
        return false;

    m_ePlayerType = static_cast<PlayerType>(ePlayerType);


    if (m_bIsLeaving && !m_ptAddr->isLoopback())
        m_bIsLeaving = FALSE;
    return true;
}
} }
#line 515



































namespace Console { namespace Online {
#line 383
const D3::Hero::Digest * PartyPlayer::GetHeroDigest() const
{

    const D3::Hero::SavedDefinition * ptHeroDefinition = GetHero();
    if (ptHeroDefinition)
    {
        return &ptHeroDefinition->digest();
    }
    if (IsLocal())
        return NULL;
    return m_ptHeroDigest;
}
} }
#line 566


































namespace Console { namespace Online {
#line 404
void PartyPlayer::Export(PartyMember & tMember, BOOL bDigests)

{

    tMember.idPlayer = m_idPlayer;
    tMember.idOnlineUser = m_idOnlineUser;
    Blizzard::String::Copy(tMember.szName, m_szName, sizeof(tMember.szName));
    tMember.bIsLocal = IsLocal();
    tMember.bIsPrimary = IsPrimary();
    tMember.bIsGuest = IsGuest();
    tMember.bIsInGame = IsInGame();
    tMember.nControllerIndex = m_nControllerIndex;

    tMember.tHeroDigest = blz::optional<D3::Hero::Digest>();
    tMember.tAccountDigest = blz::optional<D3::Account::Digest>();

    if (bDigests) {

        const D3::Hero::Digest * ptHeroDigest = GetHeroDigest();
        if (ptHeroDigest)

            tMember.tHeroDigest = *ptHeroDigest;


        const D3::Account::SavedDefinition * ptAccount = GetAccount();
        if (ptAccount)

            tMember.tAccountDigest = ptAccount->digest();
    }
}
} }
#line 634
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/Partition.h"
#include "../../Common/Definitions/HeroDefinition.h"
namespace Console {
namespace UIOnlineActions {
bool IsActiveSeason(uint32 uSeasonNum, const D3::Account::Digest & tAccountDigest); // OnlineActions.cpp:1612
}
}
namespace Console {
namespace GamerProfile {
OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx); // GamerProfileNX64.cpp:1025
}
}
#include "Matchmaking.h"
#include "PresenceManager.h"
namespace Console {
namespace Online {
// DWARF: anonymous enum declared at ConsoleParty.cpp:75 (physical line 75 holds relocated code).
enum {
    PARTY_GAME_INFO_UPDATE_INTERVAL_MS = 5000,
    MIGRATION_MAX_VALIDATION_MS = 2000,
    MIGRATION_MAX_TIME_UNTIL_FINALIZE_MS = 5000,
    MIGRATION_BECOME_HOST_DELAY_MS = 1500,
    MIGRATION_MAX_FINALIZING_TIME_MS = 5000,
    RESERVATION_DURATION_MS = 10000,
};
// Definitions at Online.cpp:4861 (Metrics), Online.cpp:4965, Online.cpp:5234 (also declared below at 2524).
namespace Metrics {
void UpdateLocalPlayerCount(uint32 uNumLocalPlayers);
}
void ADHocUpdateHost(const bdSecurityID & tSecID, const bdSecurityKey & tSecKey, const GameInfo & tGameInfo, BOOL bForceUpdateGameInfo);
PartyGameInfoChangedEvent & GetPartyGameInfoChangedEvent();
}
}
#include "VoiceChat.h"
// GamerProfileNX64.cpp:772 and :792 definitions (no header declaration in src).
namespace Console {
namespace GamerProfile {
BOOL RefreshPlayHistoryRegistrationKey(int32 nUserIndex, BOOL bUseLocalCommunicationKey);
BOOL GetUserPlayRegistrationKey(int32 nUserIndex, nn::friends::PlayHistoryRegistrationKey & OutRegistrationKey);
}
}


namespace Console { namespace Online {
void Party::FinalizeOnlineStateChange()

{




    BOOL bWasOnline = IsOnline();
    PartySwitchOnlineResult eResult = PARTY_SWITCH_ONLINE_OK;


    if (m_ePendingOnlineState >= PARTY_LAN)
    {
        NetworkMode eMinimumRequiredMode = (m_ePendingOnlineState == PARTY_LAN) ? NET_LAN : NET_ONLINE;

        if (GetNetworkMode() < eMinimumRequiredMode)
        {
            eResult = (eMinimumRequiredMode == NET_LAN) ? PARTY_SWITCH_NO_NETWORK_CONNECTION : PARTY_SWITCH_NO_INTERNET_CONNECTION;


            if (m_ePendingOnlineState >= PARTY_ONLINE)
            {

                switch (GamerProfile::GetOnlinePlayPrivilege(m_nControllerIndex))
                {
                case GamerProfile::ONLINE_PLAY_ALLOWED:
                    break;
                case GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_LOCALLY:
                case GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE:
                    eResult = PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_ONLINE;
                    break;
                case GamerProfile::ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS:
                    eResult = PARTY_SWITCH_ONLINE_PARENTAL_BLOCK;
                    break;
                case GamerProfile::ONLINE_PLAY_BLOCKED_BANNED:
                    eResult = PARTY_SWITCH_ONLINE_BANNED;
                    break;
                case GamerProfile::ONLINE_PLAY_BLOCKED_UPDATE_REQUIRED:
                    eResult = PARTY_SWITCH_ONLINE_UPDATE_REQUIRED;
                    break;
                default:
                    eResult = PARTY_SWITCH_UNKNOWN_ERROR;
                    break;
                }
            }

            m_ePendingOnlineState = (GetNetworkMode() == NET_OFFLINE || !GamerProfile::IsSignedIn(m_nControllerIndex)) ? PARTY_OFFLINE : PARTY_LAN;
        }
    }


    if (m_ePendingOnlineState != PARTY_OFFLINE)
    {
        PartySwitchOnlineResult eCheckResult = CheckOnlineStateEligibility(m_ePendingOnlineState);
        if (eCheckResult != PARTY_SWITCH_ONLINE_OK)
        {
            eResult = eCheckResult;
            m_ePendingOnlineState = PARTY_OFFLINE;
        }
    }


    if (m_ePendingOnlineState == PARTY_LAN)
        m_ePendingOnlineState = PARTY_OFFLINE;





    bdNetImpl *ptNet = bdSingleton<bdNetImpl>::getInstance();
    ptNet->getConnectionStore()->unregisterDispatchInterceptor(this);
    ptNet->getConnectionStore()->registerDispatchInterceptor(this);

    m_eOnlineState = m_ePendingOnlineState;


    // Inferred from the original unsigned range test (b.lo #3) with the || at column 41.
    if (m_eOnlineState == PARTY_OFFLINE ||
        m_eOnlineState == PARTY_ADHOC ||
        m_eOnlineState == PARTY_LAN ||


        (!bWasOnline && IsOnline()))
    {
        if (m_bRegisteredKey)
        {

            if (ptNet->getSocketRouter() != NULL)

                ptNet->getSocketRouter()->getKeyMap()->unregisterKey(m_tSecID);

            m_tSecID = bdSecurityID();
            m_tSecKey = bdSecurityKey();
            m_bRegisteredKey = FALSE;
        }
    }


    if (m_eOnlineState != PARTY_OFFLINE &&
        !m_bRegisteredKey)
    {
        bdSecurityKeyMap::initKey(m_tSecID, m_tSecKey);
        m_bRegisteredKey = bdSingleton<bdNetImpl>::getInstance()->getSocketRouter()->getKeyMap()->registerKey(m_tSecID, m_tSecKey);
    }

    if (bWasOnline != IsOnline())
    {



        for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
             nPlayerIdx != -1;
             nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
        {
            PartyPlayer &tPlayer = m_tPlayers[nPlayerIdx];

            if (GamerProfile::IsSignedIn(tPlayer.m_nControllerIndex))
            {
                // Lines 797-814 carry no line-table rows: an empty or compiled-out branch (inferred).

















            }
            else if (m_eOnlineState != PARTY_OFFLINE)
            {


                RemovePlayer(nPlayerIdx);
            }
        }

        m_bOwnsPresenceSessionKey = FALSE;
        if (IsOnline())
        {

            int32 anLocalUsers[MAX_LOCAL_PLAYERS];
            int32 nNumUsers = 0;

            for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
                 nPlayerIdx != -1;
                 nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
            {
                PartyPlayer &tPlayer = m_tPlayers[nPlayerIdx];
                anLocalUsers[nNumUsers++] = tPlayer.m_nControllerIndex;
            }

            PartyConnectionInfo tConnectionInfo(
                bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(),
                m_tSecID,
                m_tSecKey,
                TRUE);

            if (m_ptSessionManager != NULL)
            {
                m_ptSessionManager->CreateSession(
                    tConnectionInfo,
                    m_eOnlineState,
                    anLocalUsers,
                    nNumUsers,
                    OnlineService::Callback<void (int, int &)>(OnlineService::internal::method_bind_0_2<void, Party, Party *, int, int &, &Party::onCreateSession>(this)));
            }
        }
        else
        {
            if (m_ptSessionManager != NULL)
            {
                m_ptSessionManager->LeaveSession();
            }
        }
    }

    bdCommonAddrRef ptLocalAddr = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
         nPlayerIdx != -1;
         nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
    {
        PartyPlayer &tPlayer = m_tPlayers[nPlayerIdx];
        tPlayer.m_ptAddr = ptLocalAddr;
    }





    bool bUseLocalCommunicationKey = IsAdHOC();

    for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
         nPlayerIdx != -1;
         nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
    {
        PartyPlayer &tPlayer = m_tPlayers[nPlayerIdx];

        if (GamerProfile::IsSignedInOnline(tPlayer.m_nControllerIndex))
        {
            GamerProfile::RefreshPlayHistoryRegistrationKey(tPlayer.m_nControllerIndex, bUseLocalCommunicationKey);


            GamerProfile::GetUserPlayRegistrationKey(tPlayer.m_nControllerIndex, tPlayer.m_tAdditionalPlatformInfo.m_tPlayRegistrationKey);
        }
    }



    MarkGameInfoDirty();

    SendPartyInfo(NULL);

    if (VoiceChat::Get() != NULL)
    {
        VoiceChat::Get()->OnOnlineStateChanged();
    }

    QueueNotification(GetPartyOnlineStateChangedEvent(), eResult);
}
}
}

namespace Console { namespace Online {
void Party::Idle()

{
    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();

    if (IsHost() &&
        IsOnlineStateChanging() &&
        m_eConnectionState != PARTY_SHUTTING_DOWN)
    {
        if (!IsNetworkChanging())
        {
            FinalizeOnlineStateChange();
        }
    }


    if (m_eConnectionState == PARTY_SHUTTING_DOWN)
    {
        BOOL bShutdown = TRUE;
        for (PeerRef ptPeer : m_aptPeers)
        {
            bdConnectionRef ptPeerConnection = ptPeer->m_ptConnection;
            bShutdown = bShutdown && ptPeerConnection->getStatus() == bdConnection::BD_DISCONNECTED;
        }

        bShutdown = bShutdown && (!m_ptHostConnection || m_ptHostConnection->getStatus() == bdConnection::BD_DISCONNECTED);

        if (bShutdown)
        {
            m_aptPeers.clear();
            m_ptHostConnection = NULL;

            m_eConnectionState = PARTY_NOT_CONNECTED;
        }
    }


    if (m_eConnectionState != PARTY_CONNECTED) {
        if (m_eConnectionState != PARTY_NOT_CONNECTED) return;
        if (m_ptSessionManager == NULL || !m_ptSessionManager->IsBusy()) {

            sgtParties.remove(this);
        }
        return;
    }







    if (m_eMigrationState != MIGRATION_IDLE)
    {
        Blizzard::Time::Millisecond uTimeElapsed = Blizzard::Time::GetTimeElapsed(m_uMigrationStateStart, now);

        switch (m_eMigrationState)
        {
        case MIGRATION_VALIDATING:
            if (uTimeElapsed > MIGRATION_MAX_VALIDATION_MS)
            {
                if (m_ptHostConnection->getStatus() != bdConnection::BD_DISCONNECTED)
                {

                    m_ptHostConnection->close();
                }
            }
            break;

        case MIGRATION_ACTIVE:
            if (HaveAllPeersRequestedMigration())
            {
                FinalizeMigration();
            }

            else if (uTimeElapsed > MIGRATION_MAX_TIME_UNTIL_FINALIZE_MS)
            {
                FinalizeMigration();
            }
            break;

        case MIGRATION_BECOMING_HOST:

            if (uTimeElapsed > MIGRATION_BECOME_HOST_DELAY_MS)
            {
                BecomeHost();
            }
            break;

        case MIGRATION_FINALIZING:

            if (uTimeElapsed > MIGRATION_MAX_FINALIZING_TIME_MS)
            {
                BecomeHost();
            }
            break;
        }
    }





    if (IsHost())
    {

        if ((IsOnline() && GetNetworkMode() != NET_ONLINE) ||
            (IsLAN() && GetNetworkMode() == NET_OFFLINE)

            || (IsAdHOC() && GetNetworkMode() == NET_OFFLINE))
        {


            if (!IsNetworkChanging())
            {
                if (GetNetworkMode() == NET_LAN)
                {
                    SetOnlineState(PARTY_LAN);
                }
                else
                {
                    SetOnlineState(PARTY_OFFLINE);
                }
            }
        }
        else if (GetNetworkMode() == NET_ONLINE &&

            !IsNetworkChanging() &&
            !IsOnlineStateChanging())
        {

            if (m_ePreferredOnlineState != m_eOnlineState)
            {
                PartySwitchOnlineResult eResult = PartyCanSwitchOnlineState(m_ePreferredOnlineState);


                if (eResult == PARTY_SWITCH_ONLINE_OK)

                    SetOnlineState(m_ePreferredOnlineState);
            }
        }



        BOOL bPartyInfoChanged = FALSE;

        for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i))
        {
            PartyPlayer & tPlayer = m_tPlayers[i];


            if (tPlayer.IsReserved() &&
                Blizzard::Time::GetTimeElapsed(tPlayer.m_uReservationTime, Blizzard::Time::Milliseconds()) > RESERVATION_DURATION_MS)
            {

                bPartyInfoChanged = RemovePlayer(i) || bPartyInfoChanged;
            }
        }

        if (bPartyInfoChanged)

            SendPartyInfo(NULL);


        if (!IsOnlineStateChanging())
        {
            if (IsOnline() &&
                IsInGame())
            {
                if (IsNetworkRunning())
                {





                    MatchParameters tParams;
                    GetMatchParameters(tParams);
                    Matchmaker::Get()->UpdateGame(m_tSecID, m_tSecKey, tParams);
                }
            }


            else
                Matchmaker::Get()->UnregisterGame();
        }
    }


    else
        Matchmaker::Get()->UnregisterGame();


    if (IsOnline() &&
        IsNetworkRunning() &&
        !IsMigrating() &&
        CheckOnlineStateEligibility(m_eOnlineState) != PARTY_SWITCH_ONLINE_OK)
    {



        if (!IsHost())

            ForceMigrate();

        else


            SetOnlineState(PARTY_OFFLINE);
    }







    if (m_bGameInfoDirty)

        QueueNotification(GetPartyGameInfoChangedEvent());




    if (IsHost() && IsAdHOC())
    {
        GameInfo tGameInfo;
        GetGameInfo(tGameInfo);
        ADHocUpdateHost(m_tSecID, m_tSecKey, tGameInfo, m_bGameInfoDirty);
    }


    Blizzard::Time::Millisecond uTimeSinceLastGameInfoUpdate = Blizzard::Time::GetTimeElapsed(m_uLastGameInfoUpdateTime, now);
    if (uTimeSinceLastGameInfoUpdate > PARTY_GAME_INFO_UPDATE_INTERVAL_MS)

        m_bGameInfoDirty = TRUE;


    if (m_bGameInfoDirty &&
        !IsOnlineStateChanging() &&
        !IsNetworkChanging() &&
        sptParty == this)
    {
        if (IsOnline())

            UpdateSession();



        UpdatePlayHistory();


        UpdateVoiceChatSession(FALSE);

        GameInfo tGameInfo;
        GetGameInfo(tGameInfo);

        if (IsHost())

            if (IsLAN())

                UpdateLAN(tGameInfo);



        if (!IsMigrating())
        {
            PartyConnectionInfo tConnectionInfo(
                m_ptHostConnection ? m_ptHostConnection->getAddress() : bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(),
                m_tSecID,
                m_tSecKey,
                IsOnline());

            PresenceManager::SetSessionAndGamePresence(tConnectionInfo, tGameInfo);
        }


        uint32 uNumLocalPlayers = 0;
        if (IsGameStarted())
        {
            for (int32 nPlayerIndex = GetFirstLocalPlayerIndex();
                 nPlayerIndex != -1;
                 nPlayerIndex = GetNextLocalPlayerIndex(nPlayerIndex))
            {
                uNumLocalPlayers++;
            }
        }
        Metrics::UpdateLocalPlayerCount(uNumLocalPlayers);




        snGameTag = m_tGameParams.m_nTag;


        m_uLastGameInfoUpdateTime = now;
    }
    m_bGameInfoDirty = FALSE;
}
}
}
























































































































































namespace Console { namespace Online {
void Party::Leave()

{
    for (uint32 i = 0; i < MAX_LOCAL_PLAYERS; ++i) {

        if (m_apfnAddPlayerCallbacks[i]) {

            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyAddPlayerCallback>, int>::bind<&OnlineService::internal::callback_base<1, PartyAddPlayerCallback>::execute>(m_apfnAddPlayerCallbacks[i], FALSE));
            m_apfnAddPlayerCallbacks[i] = PartyAddPlayerCallback();
        }
    }

    if (m_eConnectionState != PARTY_NOT_CONNECTED && m_eConnectionState != PARTY_SHUTTING_DOWN) {






        if (m_ptHostConnection) {

            bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_LEAVING, false);
            SendToHost(ptMessage);
        }

        Cleanup();
        m_eConnectionState = PARTY_SHUTTING_DOWN;
    }

    if (m_ptSessionManager != NULL) {

        m_ptSessionManager->LeaveSession();
    }

    if (sptParty == this) {

        sptParty = NULL;
    }
}
}
}




namespace Console { namespace Online {
#line 309
void PartyPlayer::UpdateAccountAndHero()

{


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !GamerProfile::IsHeroLoaded(m_nControllerIndex))
        return;


    ::delete m_ptHeroDefinition;
    m_ptHeroDefinition = NULL;
    m_ptHeroDefinition = ::new D3::Hero::SavedDefinition(GamerProfile::GetHero(m_nControllerIndex));


    ::delete m_ptAccountDefinition;
    m_ptAccountDefinition = NULL;
    m_ptAccountDefinition = ::new D3::Account::SavedDefinition(GamerProfile::GetAccount(m_nControllerIndex));




    BOOL bIsHardcore = (m_ptHeroDefinition->digest().player_flags() & (1 << PLAYERFLAG_HARDCORE)) != 0;
    BOOL bIsSeasonal = UIOnlineActions::IsActiveSeason(m_ptHeroDefinition->digest().season_created(), m_ptAccountDefinition->digest());
    D3AccountPartition idPartition = GetPartition(bIsHardcore, bIsSeasonal);

    google::protobuf::RepeatedPtrField<D3::Account::AccountPartition> tPartitions;
    m_ptAccountDefinition->mutable_partitions()->Swap(&tPartitions);

    for (D3::Account::AccountPartition & tPartition : tPartitions)
    {
        if (tPartition.partition_id() == idPartition)
        {
            tPartition.Swap(m_ptAccountDefinition->add_partitions());
            break;
        }
    }




    m_ptAccountDefinition->mutable_partitions(0)->mutable_console_partition_data()->clear_seasonal_rollover_item();


    m_ptHeroDefinition->mutable_digest()->set_deprecated_alt_level(m_ptAccountDefinition->partitions(0).alt_level());

}
}
}
#line 1461
namespace Console { namespace Online {
#line 514
PartySwitchOnlineResult Party::CheckOnlineStateEligibility(
    PartyOnlineState eState)
{

    if (eState != PARTY_OFFLINE)
    {
        uint32 uFailedPrivileges = 0;

        for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
             nPlayerIdx != -1;
             nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
        {
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];

            if (tPlayer.m_bIsLeaving)
                continue;



            if (eState >= PARTY_ONLINE)
            {
                GamerProfile::OnlinePlayPrivilege ePrivilege = GamerProfile::GetOnlinePlayPrivilege(tPlayer.m_nControllerIndex);
                uFailedPrivileges |= 1 << ePrivilege;
            }
            else if (!GamerProfile::IsSignedIn(tPlayer.m_nControllerIndex))
                return PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_LOCALLY;
        }



        static const struct {
            GamerProfile::OnlinePlayPrivilege ePrivilege;
            PartySwitchOnlineResult eResult;
        } satPrivileges[] = {
            { GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_LOCALLY, PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_ONLINE },
            { GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE, PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_ONLINE },
            { GamerProfile::ONLINE_PLAY_BLOCKED_UPDATE_REQUIRED, PARTY_SWITCH_ONLINE_UPDATE_REQUIRED },
            { GamerProfile::ONLINE_PLAY_BLOCKED_NO_PRIVILEGE, PARTY_SWITCH_ONLINE_NO_PRIVILEGE },
            { GamerProfile::ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS, PARTY_SWITCH_ONLINE_PARENTAL_BLOCK },
            { GamerProfile::ONLINE_PLAY_BLOCKED_BANNED, PARTY_SWITCH_ONLINE_BANNED },
            { GamerProfile::ONLINE_PLAY_BLOCKED_REQUIRES_PREMIUM, PARTY_SWITCH_ONLINE_PREMIUM_SERVICE_REQUIRED },
            { GamerProfile::ONLINE_PLAY_BLOCKED_OTHER, PARTY_SWITCH_ONLINE_OTHER_XBOXONE_ISSUE },
        };


        for (int32 i = 0; i < 8; ++i)
        {
            if (uFailedPrivileges & (1 << satPrivileges[i].ePrivilege))
            {
                return satPrivileges[i].eResult;
            }
        }
    }

    return PARTY_SWITCH_ONLINE_OK;
}
}
}
#line 1522
































































namespace Console { namespace Online {

BOOL Party::RemovePlayer(bdCommonAddrRef ptAddr, int32 nControllerIndex)
{


    for (uint32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i)) {

        PartyPlayer & tPlayer = m_tPlayers[i];

        if (tPlayer.IsAddressAndIndex(ptAddr, nControllerIndex))
            return RemovePlayer(i);
    }

    return FALSE;
}
} }



























































































namespace Console { namespace Online {

void Party::SendPartyInfo(bdConnectionRef ptConnection) const
{

    if (m_aptPeers.empty())
        return;



    bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_INFO, false);
    Serialize(ptMessage->getPayload());

    if (ptConnection)
    {
        SendTo(ptConnection, ptMessage);
    }
    else
    {
        Send(ptMessage);
    }
}
} }

namespace Console { namespace Online {

void Party::SendPlayerAccountAndHeroToHost(
    PartyPlayer & tPlayer,
    SendAccountMode eMode) const
{
    if (eMode == SEND_ACCOUNT_AND_HERO)
    {
        const D3::Account::SavedDefinition & tAccountDefinition = *tPlayer.GetAccount();
        SendChunkedData(tPlayer.m_idPlayer, CHUNKED_DATA_ACCOUNT, tAccountDefinition, NULL, NULL);
    }




    const D3::Hero::SavedDefinition * ptHeroDefinition = tPlayer.GetHero();

    const D3::Hero::SavedDefinition & tHeroDefinition = *ptHeroDefinition;

    SendChunkedData(tPlayer.m_idPlayer, CHUNKED_DATA_HERO, tHeroDefinition, NULL, NULL);
}
} }




namespace Console { namespace Online {

void Party::SendHeroDigest(
    uint8 idPlayer,
    bdConnectionRef ptConnection,
    bdConnectionRef ptSkipConnection) const
{


    const PartyPlayer & tPlayer = m_tPlayers[idPlayer];

    const D3::Hero::Digest * ptDigest = tPlayer.GetHeroDigest();
    if (!ptDigest)
        return;







    SendChunkedData(
        tPlayer.m_idPlayer,
        CHUNKED_DATA_HERO_DIGEST,
        *ptDigest,
        ptConnection,
        ptSkipConnection);
}
} }

namespace Console { namespace Online {
void GameCreateServerGameConnection(bdConnectionRef ptConnection);  // ConsoleGameConnection.cpp:136
void Party::StartGameForPeer(bdConnectionRef ptConnection)
{

    GameCreateServerGameConnection(ptConnection);

    if (!ptConnection)
    {
        HandleStartGame();
    }


    for (int32 nPlayerIndex = GetFirstPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = GetNextPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];

        if (ptConnection)
        {
            if (!tPlayer.IsAddressAndIndex(ptConnection->getAddress(), tPlayer.m_nControllerIndex))
                continue;
        }
        else
        {
            if (!tPlayer.IsLocal())
                continue;
        }

        tPlayer.m_bInGame = true;


        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_PLAYER_IN_GAME, false);
        D3PartyMsgPlayerInGame tPartyMsg(tPlayer.m_idPlayer, true);
        tPartyMsg.Serialize(ptMessage->getPayload());
        Send(ptMessage);
    }
}
} }

namespace Console { namespace Online {

BOOL Party::StartGame()
{

    if (!CanStartGame() ||
        IsInGame())
        return FALSE;



    if (IsHost())
    {
        m_bIsPaused = FALSE;
        m_bIsSoftPaused = FALSE;
        m_bGameInProgress = TRUE;


        MarkGameInfoDirty();


        StartGameForPeer(NULL);


        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_GAME_STARTING, false);
        Send(ptMessage);
    }
    else
    {
        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_REQUEST_START_GAME, false);
        SendToHost(ptMessage);
    }
    return TRUE;
}
} }















#include "../../Common/GameSwig.h"
#include "../../Common/OnlineService/OnlineService.h"
namespace Console { namespace Online {
void GameClearClientGameConnection();  // ConsoleGameConnection.cpp:100
void GameClearServerConnections();  // ConsoleGameConnection.cpp:192


void Party::EndGame()

{
    if (!IsInGame())
        return;



    m_bInGame = FALSE;
    GameClearClientGameConnection();

    if (!IsHost())
    {


        for (int32 nPlayerIndex = GetFirstLocalPlayerIndex();
             nPlayerIndex != -1;
             nPlayerIndex = GetNextLocalPlayerIndex(nPlayerIndex))
        {
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
            SendPlayerAccountAndHeroToHost(tPlayer, SEND_ACCOUNT_AND_HERO);
        }
        return;
    }


    m_bIsPaused = FALSE;
    m_bIsSoftPaused = FALSE;
    m_bGameInProgress = FALSE;


    GameClearServerPendingConnections();
    GameClearServerConnections();

    if (GamerProfile::IsHeroLoaded(m_nControllerIndex))
    {
        const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(m_nControllerIndex);

        m_tGameParams.m_bResumeFromSave = FALSE;
        m_tGameParams.m_eAct = static_cast<Act>(tHeroDefinition.digest().last_played_act());
        m_tGameParams.m_snoQuest = tHeroDefinition.digest().last_played_quest();
        m_tGameParams.m_nQuestStep = tHeroDefinition.digest().last_played_quest_step();
    }

    if (m_tGameParams.m_bIsSeasonal &&
        OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE)
    {
        m_tGameParams.m_bIsSeasonal = FALSE;
    }


    for (int32 nPlayerIndex = GetFirstPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = GetNextPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
        tPlayer.m_bInGame = false;
    }

    sptParty->MarkGameInfoDirty();
    SendPartyInfo(NULL);


    for (int32 nPlayerIndex = GetFirstLocalPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = GetNextLocalPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
        SendHeroDigest(tPlayer.m_idPlayer, NULL, NULL);
    }

    bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_GAME_ENDING, false);
    Send(ptMessage);
}
} }

namespace Console { namespace Online {

bdConnectionRef Party::InternalConnect(
    bdCommonAddrRef ptAddress,
    bdSecurityID tSecurityID)
{
    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    bdConnectionStore *const ptConnectionStore = ptNet->getConnectionStore();



    bdConnectionRef ptConnection = ptConnectionStore->create(ptAddress, tSecurityID);

    if (!ptConnection ||
        !ptConnection->connect()) {

        bdNChar8 szAddress[22];
        if (ptConnection) {

            bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
        } else {


            ptAddress->getPublicAddr().toString(szAddress, sizeof(szAddress));
        }


        ptConnection = NULL;
    }

    return ptConnection;
}
} }


















namespace Console { namespace Online {

void Party::Serialize(bdBitBufferRef ptBitBuffer) const
{



    ptBitBuffer->writeUByte8(m_uNumPlayers);
    ptBitBuffer->writeUByte8(m_uMaxPlayers);
    ptBitBuffer->writeUByte8(m_eOnlineState);
    ptBitBuffer->writeBool(m_bGameInProgress);
    ptBitBuffer->writeBool(m_bIsPaused);
    ptBitBuffer->writeBool(m_bIsSoftPaused);
    ptBitBuffer->writeByte8(m_idHostPlayer);

    m_tGameParams.Serialize(ptBitBuffer);










    for (int32 i = 0; i < MAX_PEERS; ++i) {

        const PartyPlayer & tPlayer = m_tPlayers[i];

        ptBitBuffer->writeBool(tPlayer.IsValid());

        if (tPlayer.IsValid())
        {

            tPlayer.Serialize(ptBitBuffer);
        }
    }

    ptBitBuffer->writeRangedUInt32(m_aptMigrationInfo.size(), 0, MAX_PEERS, true);
    for (const bdCommonAddrRef & ptMigrationAddr : m_aptMigrationInfo)
    {
        ptMigrationAddr->serialize(ptBitBuffer);
    }
}
} }






















































































































#include "VoiceChat.h"

namespace Console { namespace Online {

void Party::onConnect(bdConnectionRef ptConnection)

{
    if (m_eConnectionState == PARTY_CONNECTED)
    {
        if (!IsHost())
        {





            PeerRef ptPeer = FindPeer(ptConnection);
            if (ptPeer)
            {
                bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_PEER_HELLO, false);
                D3PartyMsgPeerHello tHelloMessage(m_tSecID, m_tSecKey, GetNATType());
                tHelloMessage.Serialize(ptMessage->getPayload());
                SendTo(ptConnection, ptMessage);

                if (VoiceChat::Get())
                {
                    VoiceChat::Get()->PeerConnected(ptPeer, TRUE);
                }
            }




        }
    }
    else if (m_eConnectionState == PARTY_CONNECTING)
    {

        m_eConnectionState = PARTY_NEGOTIATING;




        SendToHost(m_ptJoinRequestMessage);
        m_ptJoinRequestMessage = NULL;
    }
}
} }
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
namespace Console { namespace Online {

void Party::onConnectFailed(bdConnectionRef ptConnection)

{
    BOOL bHostConnectionFailed = ptConnection == m_ptHostConnection;

    if (bHostConnectionFailed)
    {
        if (m_eConnectionState == PARTY_CONNECTING)
        {
            m_ptJoinRequestMessage = NULL;
            m_eConnectionState = PARTY_CONNECTION_FAILED;
        }
        else
        {
            m_eConnectionState = PARTY_NOT_CONNECTED;
        }




        onPartyJoinResult(JOIN_FAILED_UNABLE_TO_CONNECT);

        Leave();
    }
    else
    {
        PeerRef ptPeer = FindPeer(ptConnection);
        if (ptPeer)
        {
            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));



            auto it = blz::find(m_aptPeers.begin(), m_aptPeers.end(), ptPeer);
            m_aptPeers.erase(it);


            ptConnection->unregisterListener(this);
        }
    }
}
} }

namespace Console { namespace Online {

void Party::onDisconnect(bdConnectionRef ptConnection)

{
    bdNChar8 szAddress[22];
    bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));




    GameNetwork::ConnectionRef ptGameConnection = GameGetGameConnection(ptConnection);

    PeerRef ptPeer = FindPeer(ptConnection);
    if (ptPeer)
    {


        auto it = blz::find(m_aptPeers.begin(), m_aptPeers.end(), ptPeer);

        m_aptPeers.erase(it);

        onPeerDisconnect(ptConnection);
    }





    ptConnection->unregisterListener(this);


    if (ptConnection == m_ptHostConnection)
    {


        m_ptHostConnection = NULL;

        if (m_eConnectionState == PARTY_CONNECTED)
        {
            StartMigrating(ptConnection->getAddress());
        }
        else
        {
            onPartyJoinResult(JOIN_FAILED_UNABLE_TO_CONNECT);
            Leave();
        }
    }

    if (ptGameConnection != NULL)
    {
        ptGameConnection->onDisconnect();
    }
}
} }





































































































































































































// Online.cpp:4217/5202/5234/5282 declarations, using existing Online.h event types.
namespace Console {
namespace Online {
void UpdatePartyMembers();
PartyMemberReceivedDigestEvent & GetPartyMemberReceivedDigestEvent();
PartyGameInfoChangedEvent & GetPartyGameInfoChangedEvent();
GameStartingEvent & GetGameStartingEvent();
}
}
#include "../../../Contrib/Contrib/Zlib/zlib.h"
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdAddressMap.h"
namespace Console {
namespace Online {
// Declarations from ConsoleGameConnection.cpp:274/288 and Online.cpp:2383/3531.
void GameReceivePacketFromClient(bdConnectionRef ptConnection, bdBitBufferRef ptPayload);
BOOL GameReceivePacketFromServer(bdBitBufferRef ptPayload);
void sEventEndGame();
void PartyMerge(PartyConnectionInfo & tPartyInfo);
}
}
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "Matchmaking.h"
// ConsoleGameConnection.cpp:250 (as declared in Online.cpp:46).
namespace Console {
namespace Online {
void GameCleanupGameConnections();
}
}







namespace Console { namespace Online {

BOOL Party::onPartyJoined(
    const bdBitBufferRef ptBuffer)
{



    BOOL bOk = Deserialize(ptBuffer);

    if (!bOk)
        return FALSE;



    if (sptParty->IsHost() &&
        !sptParty->m_aptPeers.empty() &&
        (!sptParty->IsLAN() || !IsOnline()))
    {

        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_MERGE, false);
        D3PartyMsgJoinParty tJoinParty(m_ptHostConnection->getAddress(), m_tSecID, m_tSecKey);
        tJoinParty.Serialize(ptMessage->getPayload());
        sptParty->Send(ptMessage);
    }



    PartyRef ptOldParty = sptParty;
    sptParty = this;



    if (ptOldParty &&
        ptOldParty->m_tSecID == m_tSecID &&
        ptOldParty->m_tSecKey == m_tSecKey)
    {
        ptOldParty->m_bRegisteredKey = FALSE;
        m_bRegisteredKey = TRUE;
    }

    ptOldParty->Leave();
    GameCleanupGameConnections();

    Matchmaker::Get()->UnregisterGame();

    if (IsOnline())
    {
        int32 anLocalUsers[MAX_LOCAL_PLAYERS];
        int32 nNumUsers = 0;

        for (int32 nPlayerIdx = GetFirstLocalPlayerIndex();
             nPlayerIdx != -1;
             nPlayerIdx = GetNextLocalPlayerIndex(nPlayerIdx))
        {
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];

            anLocalUsers[nNumUsers++] = tPlayer.m_nControllerIndex;
        }

        if (m_ptSessionManager != NULL)
        {

            m_ptSessionManager->JoinSession(



                m_idSession.getBuffer(),
                anLocalUsers,
                nNumUsers,

                OnlineService::Callback<void (int, int &)>(OnlineService::internal::method_bind_0_2<void, Party, Party *, int, int &, &Party::onJoinSession>(this)));
        }
    }

    MarkGameInfoDirty();

    return TRUE;
}
}
}
namespace Console { namespace Online {

void Party::onPartyJoinResult(JoinResult eResult)
{
    if (eResult == JOIN_SUCCESS)
    {

        QueueNetworkEventForMainThread([]()
        {
            if (!GameIsStartUp())
            {
                GameStartMenu(GameGetCurrentAct());
            }
        });
    }
    if (m_pfnJoinCallback)
    {
        QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyJoinCallback>, const JoinResult &>::bind<&OnlineService::internal::callback_base<1, PartyJoinCallback>::execute>(m_pfnJoinCallback, eResult));
        m_pfnJoinCallback = PartyJoinCallback();
    }
    if (eResult == JOIN_SUCCESS)
    {
        QueueNotification(GetPartyJoinedEvent());
        UpdateVoiceChatSession(TRUE);
    }
}
} }


namespace Console { namespace Online {
void Party::onPeerConnect(
    const bdConnectionRef ptConnection)
{
    if (!IsHost())
    {
        return;
    }

    BOOL bPartyInfoChanged = FALSE;
    for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i))
    {
        PartyPlayer & tPlayer = m_tPlayers[i];

        if (*tPlayer.m_ptAddr == *ptConnection->getAddress())
        {

            D3PartyMsgAddPlayerResponse tResponse(tPlayer.m_nControllerIndex, tPlayer.m_idPlayer);

            bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_ADDPLAYER_RSP, false);
            tResponse.Serialize(ptMessage->getPayload());
            SendTo(ptConnection, ptMessage);

            tPlayer.ClearReservation();
            bPartyInfoChanged = TRUE;
        }
        else
        {


            SendHeroDigest(tPlayer.m_idPlayer, ptConnection, NULL);
        }
    }

    if (bPartyInfoChanged)
    {
        MarkGameInfoDirty();
        SendPartyInfo(NULL);
    }

    if (m_tGameParams.m_eGameType == GT_CHALLENGE_RIFTS)
    {
        SendChallengeRift(ptConnection);
    }
}
} }
namespace Console { namespace Online {
void GameRemoveServerGameConnection(bdConnectionRef ptConnection);  // ConsoleGameConnection.cpp:201

void Party::onPeerDisconnect(const bdConnectionRef ptConnection)

{
    if (IsHost())
    {
        GameRemoveServerGameConnection(ptConnection);

        BOOL bPartyInfoChanged = FALSE;

        for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i))
        {
            PartyPlayer & tPlayer = m_tPlayers[i];

            if (*tPlayer.m_ptAddr == *ptConnection->getAddress())
            {
                bPartyInfoChanged = RemovePlayer(i) || bPartyInfoChanged;
            }
        }

        UpdateMigrationInfo();


        if (bPartyInfoChanged)
        {
            MarkGameInfoDirty();
            SendPartyInfo(NULL);
        }
    }
}
} }

namespace Console { namespace Online {

bdBool Party::accept(
    const bdReceivedMessage & tReceivedMsg)
{
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();
    const bdMessageType eType = ptMessage->getType();

    PacketResult eResult = PACKET_OK;

    PeerRef ptPeer = FindPeer(ptConnection);

    if (!ptPeer || m_eConnectionState == PARTY_NEGOTIATING)
    {

        if (IsOffline() && m_eConnectionState != PARTY_NEGOTIATING)
        {


            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));

            return false;
        }


        switch (eType)
        {
        case D3_MSG_PARTY_JOIN_REQ:
            eResult = HandleJoinRequest(tReceivedMsg);
            break;

        case D3_MSG_PARTY_JOIN_RSP:
            eResult = HandleJoinResponse(tReceivedMsg);
            break;

        case D3_MSG_PARTY_PEER_HELLO:
            eResult = HandlePeerHello(tReceivedMsg);
            break;

        default:
            eResult = PACKET_NOT_HANDLED;
            break;
        }
        if (eResult == PACKET_CORRUPT || eResult == PACKET_INVALID)
        {


            ptConnection->disconnect();
        }

        return eResult != PACKET_NOT_HANDLED;
    }

    switch (eType)
    {
    case D3_GAMESTREAM_CLIENT:

        GameReceivePacketFromClient(ptConnection, ptMessage->getPayload());
        break;

    case D3_GAMESTREAM_SERVER:

        if (!GameReceivePacketFromServer(ptMessage->getPayload()))
        {
            eResult = PACKET_INVALID;
        }
        break;

    case D3_MSG_PARTY_LEAVING:

        ptConnection->disconnect();
        break;

    case D3_MSG_PARTY_MERGE:
    {

        D3PartyMsgJoinParty tJoinMessage(NULL, bdSecurityID(), bdSecurityKey());
        if (!tJoinMessage.Deserialize(ptMessage->getPayload()))
        {
            eResult = PACKET_CORRUPT;
            break;
        }


        for (int32 i = GetFirstPlayerIndex(); i != -1; i = GetNextPlayerIndex(i))
        {
            PartyPlayer & tPlayer = sptParty->m_tPlayers[i];
            if (!tPlayer.IsLocal() ||
                tPlayer.m_bIsLeaving)
            {

                RemovePlayer(i);
            }
        }


        for (PeerRef ptOurPeer : m_aptPeers)
        {
            ptOurPeer->m_ptConnection->unregisterListener(this);
        }
        m_aptPeers.clear();


        m_bIsHost = TRUE;
        m_ptHostConnection = NULL;

        PartyConnectionInfo tMergeInfo(
            tJoinMessage.m_ptAddr,
            tJoinMessage.m_tSecID,
            tJoinMessage.m_tSecKey,
            IsOnline());

        PartyMerge(tMergeInfo);
        break;
    }



    case D3_MSG_PARTY_INFO:
    {
        if (IsHost())
        {
            eResult = PACKET_INVALID;
        }
        else if (!Deserialize(ptMessage->getPayload()))
        {
            eResult = PACKET_CORRUPT;
        }
        break;
    }

    case D3_MSG_PARTY_ADDPLAYER_REQ:
        eResult = HandleAddPlayerRequest(tReceivedMsg);
        break;

    case D3_MSG_PARTY_ADDPLAYER_RSP:
        eResult = HandleAddPlayerResponse(tReceivedMsg);
        break;

    case D3_MSG_PARTY_REMOVEPLAYER:
        eResult = HandleRemovePlayerRequest(tReceivedMsg);
        break;

    case D3_MSG_PARTY_UPDATEPLAYERS:
        eResult = HandleUpdatePlayersRequest(tReceivedMsg);
        break;

    case D3_MSG_PARTY_CHUNKED_DATA:
    {
        if (ptConnection->getAddress()->isLoopback())
        {
            break;
        }


        PartyPlayer * ptPlayer = NULL;
        BOOL bComplete;
        PartyChunkedDataType eType = CHUNKED_DATA_INVALID;
        eResult = ReceiveChunkedData(ptConnection, ptMessage, eType, ptPlayer, bComplete);
        if (eResult != PACKET_OK)
        {
            break;
        }

        if (!bComplete)
        {
            break;
        }


        uLongf uUncompressedDataSize = ptPlayer->m_uChunkUncompressedSize;

        blz::string szUncompressedData;
        uLongf uExpectedUncompressedDataSize = uUncompressedDataSize;
        int32 nResult = uncompress(
            (Bytef *)szUncompressedData.modify_data(uUncompressedDataSize), &uUncompressedDataSize,
            (const Bytef *)ptPlayer->m_szChunkDataBuffer.data(), ptPlayer->m_szChunkDataBuffer.size());

        ptPlayer->ReleaseChunkedData();

        if (nResult != Z_OK ||
            uUncompressedDataSize != uExpectedUncompressedDataSize)
        {
            eResult = PACKET_CORRUPT;
            break;
        }


        switch (eType)
        {
        case CHUNKED_DATA_ACCOUNT:
            if (IsHost())
            {

                ::delete ptPlayer->m_ptAccountDefinition;
                ptPlayer->m_ptAccountDefinition = ::new D3::Account::SavedDefinition();
                if (!ptPlayer->m_ptAccountDefinition->ParseFromString(szUncompressedData))
                {
                    eResult = PACKET_CORRUPT;
                    break;
                }
            }
            break;







        case CHUNKED_DATA_HERO:
            if (IsHost())
            {


                ::delete ptPlayer->m_ptHeroDefinition;
                ptPlayer->m_ptHeroDefinition = ::new D3::Hero::SavedDefinition();
                if (!ptPlayer->m_ptHeroDefinition->ParseFromString(szUncompressedData))
                {
                    eResult = PACKET_CORRUPT;
                    break;
                }




                SendHeroDigest(ptPlayer->m_idPlayer, NULL, ptConnection);
                QueueNotification(GetPartyMemberReceivedDigestEvent(), int32(ptPlayer->m_idPlayer));
            }
            break;





        case CHUNKED_DATA_HERO_DIGEST:
            if (!IsHost())
            {
                ::delete ptPlayer->m_ptHeroDigest;
                ptPlayer->m_ptHeroDigest = ::new D3::Hero::Digest();
                if (!ptPlayer->m_ptHeroDigest->ParseFromString(szUncompressedData))
                {
                    ::delete ptPlayer->m_ptHeroDigest; ptPlayer->m_ptHeroDigest = NULL;
                    eResult = PACKET_CORRUPT;
                    break;
                }


                UpdatePartyMembers();

                QueueNotification(GetPartyMemberReceivedDigestEvent(), int32(ptPlayer->m_idPlayer));
            }
            break;






        case CHUNKED_DATA_CHALLENGE_RIFT:
            if (!IsHost())
            {
                m_tGameParams.m_tWeeklyChallengeData.Clear();
                if (!m_tGameParams.m_tWeeklyChallengeData.ParseFromString(szUncompressedData))
                {
                    m_tGameParams.m_tWeeklyChallengeData.Clear();
                    eResult = PACKET_CORRUPT;
                    break;
                }


                QueueNotification(GetPartyGameInfoChangedEvent());
            }
            break;

        default:
            eResult = PACKET_CORRUPT;
            break;
        }
        break;
    }





    case D3_MSG_PARTY_START_GAME:
        if (!IsHost())
        {
            HandleStartGame();
        }
        break;

    case D3_MSG_PARTY_REQUEST_START_GAME:
        if (IsHost())
        {
            if (m_bInGame && m_bGameInProgress)
            {

                StartGameForPeer(ptConnection);

                bdMessageRef ptStartMessage = ::new bdMessage(D3_MSG_PARTY_START_GAME, false);
                SendTo(ptConnection, ptStartMessage);
            }
            else
            {
                bdMessageRef ptEndMessage = ::new bdMessage(D3_MSG_PARTY_GAME_ENDING, false);
                SendTo(ptConnection, ptEndMessage);
            }
        }
        break;

    case D3_MSG_PARTY_GAME_STARTING:
        if (!IsHost())
        {
            m_bIsPaused = FALSE;
            m_bIsSoftPaused = FALSE;
            m_bGameInProgress = TRUE;
            if (m_bInGame)
            {

                bdMessageRef ptRequestMessage = ::new bdMessage(D3_MSG_PARTY_REQUEST_START_GAME, false);
                SendToHost(ptRequestMessage);
            }
            else
            {
                QueueNotification(GetGameStartingEvent());
            }
        }
        break;

    case D3_MSG_PARTY_GAME_ENDING:
        if (!IsHost())
        {
            m_bGameInProgress = FALSE;
            QueueNetworkEventForMainThread(sEventEndGame);
        }
        break;

    case D3_MSG_PARTY_PLAYER_IN_GAME:
        if (!IsHost())
        {
            D3PartyMsgPlayerInGame tMessage(0, FALSE);
            if (!tMessage.Deserialize(ptMessage->getPayload()))
            {
                eResult = PACKET_CORRUPT;
            }
            else if (tMessage.m_idPlayer < MAX_PEERS)
            {
                PartyPlayer & tPlayer = m_tPlayers[tMessage.m_idPlayer];
                tPlayer.m_bInGame = tMessage.m_bInGame;
            }
        }
        break;

    case D3_MSG_PARTY_MIGRATE:
    {
        D3PartyMsgMigrationRequest tMessage;
        if (!tMessage.Deserialize(ptMessage->getPayload()))
        {
            eResult = PACKET_CORRUPT;
            break;
        }

        bdNChar8 szAddress[22];
        bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));


        if (IsHost())
        {
            eResult = PACKET_INVALID;
            break;
        }



        if (m_eMigrationState == MIGRATION_IDLE)
        {
            if (m_ptHostConnection &&
                *m_ptHostConnection->getAddress() != *tMessage.m_ptHostAddress)
            {
                break;
            }




            SetMigrationState(MIGRATION_VALIDATING);

            bdMessageRef ptRequestMessage = ::new bdMessage(D3_MSG_PARTY_VALIDATE_REQ, false);
            SendToHost(ptRequestMessage);
        }

        if (m_eMigrationState == MIGRATION_VALIDATING ||
            m_eMigrationState == MIGRATION_ACTIVE)
        {

            ptPeer->m_bHasRequestedMigration = TRUE;
        }
        break;
    }





    case D3_MSG_PARTY_VALIDATE_REQ:
        if (IsHost())
        {
            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));


            bdMessageRef ptValidateMessage = ::new bdMessage(D3_MSG_PARTY_VALIDATE_RSP, false);
            SendTo(ptConnection, ptValidateMessage);
        }
        break;

    case D3_MSG_PARTY_VALIDATE_RSP:
        if (!IsHost())
        {
            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));


            if (m_eMigrationState == MIGRATION_VALIDATING)
            {

                for (auto it = m_aptPeers.begin(); it != m_aptPeers.end();)
                {
                    PeerRef ptPeerToDrop = *it;
                    if (ptPeerToDrop->m_bHasRequestedMigration)
                    {
                        it = m_aptPeers.erase(it);
                        CloseConnection(ptPeerToDrop->m_ptConnection);
                    }
                    else
                    {
                        ++it;
                    }
                }

                SetMigrationState(MIGRATION_IDLE);
            }
        }
        break;





    case D3_MSG_PARTY_IAMHOST:
        eResult = HandleIAmHost(tReceivedMsg);
        break;

    case D3_MSG_PARTY_VOICECHAT_DATA:
        VoiceChat::ReceiveRemoteVoiceChatData(ptPeer->m_idPeer, tReceivedMsg.getMessage(), tReceivedMsg.getConnection());
        break;

    case D3_MSG_PARTY_VOICECHAT_RELIABLE_DATA:
        VoiceChat::ReceiveRemoteVoiceChatReliableData(ptPeer->m_idPeer, tReceivedMsg.getMessage(), tReceivedMsg.getConnection());
        break;

    case D3_MSG_PARTY_VOICECHAT_MUTEUPDATE:
        VoiceChat::ReceiveVoiceChatMuteUpdate(ptPeer->m_idPeer, tReceivedMsg.getMessage(), tReceivedMsg.getConnection());
        break;

    case D3_MSG_PARTY_VOICECHAT_MICDETECTED:
        VoiceChat::ReceiveVoiceChatMicDetected(ptPeer->m_idPeer, tReceivedMsg.getMessage(), tReceivedMsg.getConnection());
        break;

    default:
    {
        bdNChar8 szAddress[22];
        bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
        eResult = PACKET_NOT_HANDLED;
        break;
    }
    }

    if (eResult == PACKET_CORRUPT || eResult == PACKET_INVALID)
    {

        bdNChar8 szAddress[22];
        bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));

        ptConnection->disconnect();
    }


    return eResult != PACKET_NOT_HANDLED;
}
} }






#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdReceivedMessage.h"
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdAddressMap.h"


namespace Console { namespace Online {

PacketResult Party::HandleJoinRequest(
    const bdReceivedMessage & tReceivedMsg)
{
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();

    bdNChar8 szAddress[22];
    bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));



    if (IsOffline())
        return PACKET_INVALID;




    if (sgtParties.size() > 1)
        return PACKET_INVALID;





    if (!IsHost())
        return PACKET_INVALID;






    D3PartyJoinRequest tJoinRequest(false, BD_NAT_UNKNOWN, JOIN_TYPE_NORMAL);
    if (!tJoinRequest.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;




    D3PartyJoinResponse tJoinResponse(JOIN_SUCCESS);
    JoinResult eResult = onConnectRequest(tJoinRequest, tJoinResponse);
    tJoinResponse.m_uResult = eResult;

    bdMessageRef ptJoinReply = ::new bdMessage(D3_MSG_PARTY_JOIN_RSP, false);
    bdBitBufferRef ptBuffer = ptJoinReply->getPayload();

    if (eResult == JOIN_SUCCESS)
    {



        tJoinResponse.AddPeers(m_aptPeers);
        tJoinResponse.Serialize(ptBuffer);


        PeerRef ptPeer = AddPeer(ptConnection, static_cast<bdNATType>(tJoinRequest.m_uNATType));

        UpdateMigrationInfo();
        Serialize(ptBuffer);

        SendTo(ptConnection, ptJoinReply);


        onPeerConnect(ptConnection);

        if (VoiceChat::Get())
        {
            UpdateVoiceChatSession(FALSE);
            VoiceChat::Get()->PeerConnected(ptPeer, FALSE);
        }
    }
    else
    {


        ptConnection->unregisterListener(this);

        tJoinResponse.Serialize(ptBuffer);

        ScopedNetworkSendLock tSendLock;
        ptConnection->send(ptJoinReply, true);
        ptConnection->disconnect();
    }

    return PACKET_OK;
}
} }




















































































































































#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdReceivedMessage.h"

namespace Console { namespace Online {

PacketResult Party::HandleAddPlayerRequest(
    const bdReceivedMessage & tReceivedMsg)
{
    if (!IsHost())
        return PACKET_INVALID;



    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();
    const bdCommonAddrRef ptAddr = ptConnection->getAddress();

    D3PartyMsgAddPlayerRequest tRequest(-1, "", AdditionalPlatformInformation(), 0, PLAYER_REGULAR, -1, FALSE, FALSE);
    if (!tRequest.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;



    BOOL bPrimaryInGame = FALSE;
    BOOL bAlreadyInParty = FALSE;
    for (int32 nPlayerIndex = GetFirstPlayerIndex();
         !bAlreadyInParty && nPlayerIndex != -1;
         nPlayerIndex = GetNextPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
        bAlreadyInParty = tPlayer.IsAddressAndIndex(ptAddr, tRequest.m_nControllerIndex);

        if (!bAlreadyInParty &&
            tPlayer.m_ePlayerType == PLAYER_PRIMARY &&
            tPlayer.IsAddressAndIndex(ptAddr, tPlayer.m_nControllerIndex))
        {
            bPrimaryInGame = tPlayer.m_bInGame;
        }
    }


    D3PartyMsgAddPlayerResponse tResponse(tRequest.m_nControllerIndex, -1);
    PartyPlayer * ptPlayer = bAlreadyInParty ? NULL : AddPlayer(ptAddr, tRequest);
    if (ptPlayer)
    {

        tResponse.m_idPlayer = ptPlayer->m_idPlayer;


        ptPlayer->m_bInGame = bPrimaryInGame;

        SendPartyInfo(NULL);
    }

    bdMessageRef ptAddPlayerResponse = ::new bdMessage(D3_MSG_PARTY_ADDPLAYER_RSP, false);
    tResponse.Serialize(ptAddPlayerResponse->getPayload());
    SendTo(ptConnection, ptAddPlayerResponse);

    return PACKET_OK;
}
} }




























































#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdReceivedMessage.h"
namespace Console { namespace Online {

PacketResult Party::HandleRemovePlayerRequest(const bdReceivedMessage & tReceivedMsg)
{

    if (!IsHost())
        return PACKET_INVALID;



    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();

    D3PartyMsgRemovePlayer tRequest(0);
    if (tRequest.Deserialize(ptMessage->getPayload()))
    {



        if (RemovePlayer(
                ptConnection->getAddress(),
                tRequest.m_nControllerIndex))
        {
            SendPartyInfo(NULL);
        }
    }
    return PACKET_OK;
}




PacketResult Party::HandleUpdatePlayersRequest(const bdReceivedMessage & tReceivedMsg)
{

    if (!IsHost())
        return PACKET_INVALID;



    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();

    D3PartyMsgUpdatePlayers tRequest;
    if (!tRequest.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;



    for (uint32 i = 0; i < tRequest.m_uNumPlayers; ++i)
    {
        D3PartyMsgUpdatePlayers::PlayerInfo & tPlayerInfo = tRequest.m_tPlayerInfo[i];

        int32 nPlayerIndex = FindPlayer(ptConnection->getAddress(), tPlayerInfo.m_nControllerIndex);
        if (nPlayerIndex != -1)
        {
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
            Blizzard::String::Copy(tPlayer.m_szName, tPlayerInfo.m_szPlayerName, sizeof(tPlayer.m_szName));
            tPlayer.m_idOnlineUser = tPlayerInfo.m_idOnlineUser;
            tPlayer.m_bBadReputation = tPlayerInfo.m_bBadReputation;
        }
    }
    SendPartyInfo(NULL);
    return PACKET_OK;
}
} }
#include "PresenceManager.h"
namespace Console { namespace Online {
void sEventStartGame(uint32 idGameConnection, GameParams tGameParams);  // Online.cpp:2299
uint32 GameCreateClientGameConnection(bdConnectionRef ptHostConnection);  // ConsoleGameConnection.cpp:76

PacketResult Party::HandleStartGame()

{
    m_bInGame = TRUE;
    MarkGameInfoDirty();


    uint32 idConnection = GameCreateClientGameConnection(m_ptHostConnection);

    QueueNetworkEventForMainThread(OnlineService::internal::function_binder_2<void, uint32, GameParams>::bind<&sEventStartGame>(idConnection, m_tGameParams));

    PresenceManager::SetListingEnabled(FALSE);
    return PACKET_OK;
}
} }

#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
namespace Console { namespace Online {

PacketResult Party::HandleIAmHost(
    const bdReceivedMessage & tReceivedMsg)
{
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();

    bdNChar8 szAddress[22];
    bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));


    if (IsHost())
        return PACKET_INVALID;




    if (m_eMigrationState != MIGRATION_FINALIZING && m_eMigrationState != MIGRATION_ACTIVE)
        return PACKET_INVALID;






    D3PartyMsgMigrationIAmHost tIAmHostMsg;
    if (!tIAmHostMsg.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;




    for (blz::vector<PeerRef>::iterator it = m_aptPeers.begin(); it != m_aptPeers.end();)
    {
        PeerRef ptPeer = *it;
        bdCommonAddrRef ptPeerAddr = ptPeer->m_ptConnection->getAddress();

        D3PartyMsgMigrationIAmHost::Peer * hostPeerIt = blz::find_if(tIAmHostMsg.m_atPeers.begin(), tIAmHostMsg.m_atPeers.end(),
            [ptPeerAddr](const D3PartyMsgMigrationIAmHost::Peer & tPeer)
            {
                return *tPeer.ptAddr == *ptPeerAddr;
            });

        if (hostPeerIt == tIAmHostMsg.m_atPeers.end())
        {
            if (ptPeerAddr->isLoopback())
                return PACKET_INVALID;




            bdAddressMap::addrToString(ptPeer->m_ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));


            it = m_aptPeers.erase(it);
            CloseConnection(ptPeer->m_ptConnection);
            continue;
        }

        ++it;
    }

    SetMigrationState(MIGRATION_IDLE);


    for (D3PartyMsgMigrationIAmHost::Peer & tMsgPeer : tIAmHostMsg.m_atPeers)
    {
        bdCommonAddrRef & ptPeerAddr = tMsgPeer.ptAddr;
        bdNATType eNATType = static_cast<bdNATType>(tMsgPeer.uNATType);

        if (!ptPeerAddr->isLoopback() &&
            !FindPeer(ptPeerAddr))
        {

            bdConnectionRef ptPeerConnection = InternalConnect(ptPeerAddr, m_tSecID);
            if (ptPeerConnection)
            {
                AddPeer(ptPeerConnection, eNATType);
            }
        }
    }


    m_ptHostConnection = ptConnection;


    for (int32 nPlayerIndex = GetFirstPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = GetNextPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];
        tPlayer.ReleaseChunkedData();
    }


    for (int32 nLocalPlayerIndex = GetFirstLocalPlayerIndex();
         nLocalPlayerIndex != -1;
         nLocalPlayerIndex = GetNextLocalPlayerIndex(nLocalPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nLocalPlayerIndex];

        if (tPlayer.m_bIsLeaving)
        {
            D3PartyMsgRemovePlayer tLeaveRequest(tPlayer.m_nControllerIndex);

            bdMessageRef ptRemoveMessage = ::new bdMessage(D3_MSG_PARTY_REMOVEPLAYER, false);
            tLeaveRequest.Serialize(ptRemoveMessage->getPayload());
            SendToHost(ptRemoveMessage);
        }
        else
        {
            SendPlayerAccountAndHeroToHost(tPlayer, SEND_ACCOUNT_AND_HERO);
        }
    }

    return PACKET_OK;
}
} }































































#include "../../../Contrib/Contrib/zlib/zlib.h"
#include "../../../Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/message.h"
#include "../../Sigma/Math/MathGeneral.h"
namespace Console { namespace Online {
extern Blizzard::Lock::RMutex stNetworkSendLock;  // DWARF: Online.cpp:417; no header declaration recorded.




void Party::SendChunkedData(
    bdByte8 idPlayer,
    PartyChunkedDataType eType,
    const google::protobuf::Message & tMessage,
    bdConnectionRef ptConnection,
    bdConnectionRef ptSkipConnection) const
{
    if (m_aptPeers.empty())
        return;



    blz::string szDefinitionBuffer;
    BOOL bProtoCompiled = tMessage.SerializeToString(&szDefinitionBuffer);

    if (!bProtoCompiled)
        return;



    uLongf uCompressedLength = Math::Max<uLongf>(compressBound(szDefinitionBuffer.size()), 1);

    blz::string szCompressedData;
    int nResult = compress(
        reinterpret_cast<Bytef *>(szCompressedData.modify_data(uCompressedLength)), &uCompressedLength,
        reinterpret_cast<const Bytef *>(szDefinitionBuffer.data()), szDefinitionBuffer.size());

    if (nResult != Z_OK)
        return;



    uint8 * pauRead = reinterpret_cast<uint8 *>(szCompressedData.data());
    uint32 uSizeRemaining = uCompressedLength;

    do {

        D3PartyMsgPlayerDataChunk tChunk(idPlayer, eType, szDefinitionBuffer.size());

        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_CHUNKED_DATA, false);

        uint32 uWriteSize = tChunk.Serialize(ptMessage->getPayload(), pauRead, uSizeRemaining);


        pauRead += uWriteSize;
        uSizeRemaining -= uWriteSize;

        if (ptConnection.notNull()) {


            SendTo(ptConnection, ptMessage);
        }
        else if (IsHost()) {


            Send(ptMessage);
        }
        else {


            SendToHost(ptMessage);
        }
    } while (uSizeRemaining);
}











































BOOL Party::SendTo(
    bdConnectionRef ptConnection,
    bdMessageRef ptMessage) const
{




    Blizzard::Lock::ScopedRMutex tSendLock(stNetworkSendLock);
    BOOL bMessageSent = ptConnection->send(ptMessage, true);
    return bMessageSent;
}
} }
















#include "../../Common/Player.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Definitions/HeroDefinition.h"
namespace Console { namespace Online {

void Party::GetGameInfo(GameInfo & tGameInfo)
{

    Blizzard::String::Copy(tGameInfo.m_szUsername, m_tPlayers[m_nControllerIndex].m_szName, sizeof(tGameInfo.m_szUsername));
    tGameInfo.m_eOnlineState = m_eOnlineState;
    tGameInfo.m_bInGame = IsInGame();
    tGameInfo.m_bIsChallenge = m_tGameParams.m_eGameType == GT_CHALLENGE_RIFTS;
    if (!tGameInfo.m_bIsChallenge) {
        tGameInfo.m_bIsHardcore = m_tGameParams.m_bIsHardcore;
        tGameInfo.m_bIsSeasonal = m_tGameParams.m_bIsSeasonal;
        tGameInfo.unk_14 = m_tGameParams.unk_14;
        tGameInfo.m_uSeason = m_tGameParams.m_uSeason;
        tGameInfo.m_eAct = m_tGameParams.m_eAct;
        tGameInfo.m_snoQuest = m_tGameParams.m_snoQuest;
        tGameInfo.m_nHandicap = m_tGameParams.m_nHandicap;
    }

    tGameInfo.m_uNumPlayers = 0;

    for (int32 nPlayerIndex = GetFirstPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = GetNextPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIndex];

        if (tPlayer.IsLocal() && tPlayer.m_bIsLeaving)
            continue;



        GameInfo::Player & tGameInfoPlayer = tGameInfo.m_tPlayers[tGameInfo.m_uNumPlayers++];

        tGameInfoPlayer.m_idUser = tPlayer.m_idOnlineUser;
        Blizzard::String::Copy(tGameInfoPlayer.m_tUserPresence.m_szName, tPlayer.m_szName, sizeof(tGameInfoPlayer.m_tUserPresence.m_szName));




        const D3::Hero::Digest * tDigest = tPlayer.GetHeroDigest();

        if (tGameInfo.m_bIsChallenge && m_tGameParams.m_tWeeklyChallengeData.has_hero_snapshot())

            tDigest = &m_tGameParams.m_tWeeklyChallengeData.hero_snapshot().digest();


        if (tDigest) {

            tGameInfoPlayer.m_tUserPresence.m_eClass = PlayerClassFromGBID(tDigest->gbid_class());
            tGameInfoPlayer.m_tUserPresence.m_bFemale = (tDigest->player_flags() & (1 << PLAYERFLAG_FEMALE)) != 0;
            tGameInfoPlayer.m_tUserPresence.m_uLevel = tDigest->level();
            tGameInfoPlayer.m_tUserPresence.m_uParagonLevel = tDigest->deprecated_alt_level();
        }
        else {

            tGameInfoPlayer.m_tUserPresence = UserPresence();
        }
    }
}
} }

namespace Console { namespace Online {

void Party::UpdateLAN(
    const GameInfo & tGameInfo)
{
    const char * szHeroName = "<UNKNOWN>";

    int32 nHostPlayer;
    for (nHostPlayer = GetFirstLocalPlayerIndex();
         nHostPlayer != -1;
         nHostPlayer = GetNextLocalPlayerIndex(nHostPlayer))
    {
        PartyPlayer & tPlayer = m_tPlayers[nHostPlayer];
        if (tPlayer.m_nControllerIndex == m_nControllerIndex)
        {






            szHeroName = tPlayer.m_szName;
            break;
        }
    }

    LANUpdateGameInfo(
        szHeroName,
        m_tSecID,
        m_tSecKey,
        tGameInfo);
}
} }














































































namespace Console { namespace Online {

void Party::UpdatePlayHistory()

{
    GamerProfile::PlayerHistoryRecord atCurrentPlayersToRegister[MAX_PEERS];
    uint32 uNumCurrentPlayersToRegister = 0;

    for (int32 nPlayerIdx = GetFirstPlayerIndex();
         nPlayerIdx != -1;
         nPlayerIdx = GetNextPlayerIndex(nPlayerIdx))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];
        if (tPlayer.IsReserved())
        {
            continue;
        }


        atCurrentPlayersToRegister[uNumCurrentPlayersToRegister++] = GamerProfile::PlayerHistoryRecord(tPlayer.m_tAdditionalPlatformInfo.m_tPlayRegistrationKey, tPlayer.m_szName);
    }


    for (int32 nLocalPlayerIdx = GetFirstLocalPlayerIndex();
         nLocalPlayerIdx != -1;
         nLocalPlayerIdx = GetNextPlayerIndex(nLocalPlayerIdx))
    {
        PartyPlayer & tLocalPlayer = m_tPlayers[nLocalPlayerIdx];
        GamerProfile::UpdatePlayHistoryRecord(tLocalPlayer.m_nControllerIndex, atCurrentPlayersToRegister, uNumCurrentPlayersToRegister);
    }
}
} }

#include "ConsoleGameConnection.h"
#include "LobbyService.h"
// Defined in CBlizzconTimer.cpp:147; no header declaration is recorded in DWARF.
extern void CBlizzconDeferReturnToStartScreen(); // CBlizzconTimer.cpp:147
#include "../../Common/Game.h"
#include "../../Common/GameSwig.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_sort.h"













// No concrete DIE and only an inlined instance (Idle cpp981): declared inline.
namespace Console { namespace Online {
inline BOOL Party::HaveAllPeersRequestedMigration()
{

    for (PeerRef ptPeer : m_aptPeers)
    {
        if (!ptPeer->m_bHasRequestedMigration)
        {
            return FALSE;
        }
    }

    return TRUE;
}
}
}
namespace Console { namespace Online {

void Party::UpdateMigrationInfo()

{
    blzext::fixed_vector<PeerRef, MAX_PEERS> aptPotentialNewHosts;


    for (PeerRef ptPeer : m_aptPeers)
    {
        bdConnectionRef ptConnection = ptPeer->m_ptConnection;
        bdNATType eNATType = ptPeer->m_eNATType;













        aptPotentialNewHosts.push_back(ptPeer);
    }

    blz::stable_sort(aptPotentialNewHosts.begin(), aptPotentialNewHosts.end(),
        [](const PeerRef & left, const PeerRef & right)
        {
            return left->m_eNATType < right->m_eNATType;
        });


    m_aptMigrationInfo.clear();
    for (PeerRef ptPeer : aptPotentialNewHosts)
    {
        bdCommonAddrRef ptAddr = ptPeer->m_ptConnection->getAddress();
        m_aptMigrationInfo.push_back(ptAddr);
    }
}

void PartyLeaveAndRemake(uint32 uFlags); // Online.cpp:2944


void Party::FailMigration()
{
    SetMigrationState(MIGRATION_DISABLED);


    QueueNetworkEventForMainThread([]()
    {
        PartyLeaveAndRemake(1);
        if (!GameIsStartUp())
        {
            GameStartMenu(ACT_5);
        }
    });

    QueueNotification(GetPartyMigratingEvent(), MIGRATION_NOT_POSSIBLE);
}
}  // namespace Online
}  // namespace Console

namespace Console { namespace Online {
void Party::StartMigrating(
    bdCommonAddrRef ptHostAddress)
{
    if (m_eMigrationState != MIGRATION_IDLE && m_eMigrationState != MIGRATION_VALIDATING)
    {
        return;
    }


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && IsInGame())
    {
        CBlizzconDeferReturnToStartScreen();
        return;
    }

    if (m_tGameParams.m_eGameType == GT_CHALLENGE_RIFTS)
    {
        FailMigration();
        return;
    }
    if (IsGameStarted() && !IsInGame())
    {
        FailMigration();
        return;
    }

    if (IsInGame())
    {
        QueueNetworkEventForMainThread(sSynchronizePlayerHeroes);

        if (m_tGameParams.m_bIsSeasonal &&
            GetSecondsUntilSeasonEnd() <= 60)
        {

            FailMigration();
            return;
        }
    }

    SetMigrationState(MIGRATION_ACTIVE);

    D3PartyMsgMigrationRequest tMessage;
    tMessage.m_ptHostAddress = ptHostAddress;

    bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_MIGRATE, false);
    tMessage.Serialize(ptMessage->getPayload());
    Send(ptMessage);



    GameClientConnectionMigrated();
}
} }




















































namespace Console { namespace UIOnlineActions { void SetGameParamsForHero(const D3::Account::SavedDefinition & tAccount, const D3::Hero::SavedDefinition & tHero, Online::GameParams & tParams); } } // OnlineActions.cpp:480

namespace Console { namespace Online {

void Party::BecomeHost()
{



    SetMigrationState(MIGRATION_IDLE);

    m_bIsHost = TRUE;
    m_bIsPaused = m_bIsSoftPaused = FALSE;

    m_ptHostConnection = NULL;



    for (int32 nPlayerIdx = GetFirstPlayerIndex();
         nPlayerIdx != -1;
         nPlayerIdx = GetNextPlayerIndex(nPlayerIdx))
    {
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];

        BOOL bRemove;


        if (tPlayer.IsLocal())
        {
            bRemove = tPlayer.m_bIsLeaving;
        }

        else
        {
            bRemove = !FindPeer(tPlayer.m_ptAddr);
        }

        if (bRemove)
        {
            RemovePlayer(nPlayerIdx);
        }
        else
        {
            tPlayer.ReleaseChunkedData();

            ::delete tPlayer.m_ptHeroDigest; tPlayer.m_ptHeroDigest = NULL;

            if (tPlayer.IsLocal(m_nControllerIndex))
            {
                m_idHostPlayer = nPlayerIdx;
            }
        }
    }


    for (PeerRef ptPeer : m_aptPeers)
    {
        ptPeer->m_bHasRequestedMigration = FALSE;
    }










    if (!m_aptPeers.empty())
    {
        D3PartyMsgMigrationIAmHost tIAmHostMsg;
        tIAmHostMsg.AddSelf(bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(), GetNATType());
        tIAmHostMsg.AddPeers(m_aptPeers);

        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_IAMHOST, false);
        tIAmHostMsg.Serialize(ptMessage->getPayload());
        Send(ptMessage);

        UpdateMigrationInfo();
    }

    if (IsInGame())
    {
        m_bInGame = FALSE;
        m_tGameParams.m_bResumeFromSave = TRUE;

        if (!GamerProfile::IsHeroLoaded(m_nControllerIndex))
        {
            m_tGameParams.m_bResumeFromSave = FALSE;
        }
        else
        {
            const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(m_nControllerIndex);

            if (m_tGameParams.m_eAct == ACT_OPEN_WORLD)
            {
                m_tGameParams.m_bResumeFromSave = FALSE;
                m_tGameParams.m_ePreferredAdventureAct = tHeroDefinition.console_data().last_played_adventure_mode_act();
            }
            else if (tHeroDefinition.digest().last_played_act() != m_tGameParams.m_eAct ||
                     tHeroDefinition.digest().last_played_quest() != m_tGameParams.m_snoQuest ||
                     tHeroDefinition.digest().last_played_quest_step() != m_tGameParams.m_nQuestStep)
            {
                m_tGameParams.m_bResumeFromSave = FALSE;
            }
        }


        SendPartyInfo(NULL);
        StartGame();
    }
    else
    {

        const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(m_nControllerIndex);
        const D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(m_nControllerIndex);
        UIOnlineActions::SetGameParamsForHero(tAccountDefinition, tHeroDefinition, m_tGameParams);
        SendPartyInfo(NULL);
    }

    if (IsOnline() &&
        GetNetworkMode() == NET_ONLINE)
    {
        PartyConnectionInfo tConnectionInfo(
            bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(),
            m_tSecID,
            m_tSecKey, FALSE);






























        m_bOwnsPresenceSessionKey = TRUE;
    }

    MarkGameInfoDirty();
}
} }

























#include "Matchmaking.h"
#include "../../Common/Attributes/FastAttribConstants.h"
namespace Console { namespace Online {

void Party::GetMatchParameters(MatchParameters & tParams) const
{

    tParams = MatchParameters();

    tParams.bIsPublic = IsPublic() && m_bMatchmakingEnabled;
    tParams.uNumPlayers = m_uNumPlayers;
    tParams.eGameType = m_tGameParams.m_eGameType;

    if (tParams.eGameType == GT_CHALLENGE_RIFTS) {

        if (GetNetworkTimestamp() >= Blizzard::Time::FromUnixTime64(m_tGameParams.m_tWeeklyChallengeConfig.challenge_end_unix_time_console()))
            tParams.bIsPublic = FALSE;



        tParams.nLevel = HERO_LEVEL_MAX;
        tParams.eAct = ACT_1;
        tParams.snoQuest = m_tGameParams.m_tWeeklyChallengeConfig.challenge_number();
    } else {

        tParams.nLevel = m_tGameParams.m_nLevel;
        tParams.bHardcore = m_tGameParams.m_bIsHardcore;
        tParams.bSeasonal = m_tGameParams.m_bIsSeasonal;
        tParams.eAct = m_tGameParams.m_eAct;
        tParams.snoQuest = m_tGameParams.m_snoQuest;
        tParams.nHandicap = m_tGameParams.m_nHandicap;
        tParams.nTag = m_tGameParams.m_nTag;
    }


    tParams.bBadReputation = FALSE;

    for (int32 i = GetFirstPlayerIndex();
         i != -1 && !tParams.bBadReputation;
         i = GetNextPlayerIndex(i))
    {
        const PartyPlayer & tPlayer = m_tPlayers[i];
        tParams.bBadReputation = tPlayer.m_bBadReputation;
    }
}
} }
































namespace Console { namespace Online {

PeerRef Party::FindPeer(bdConnectionRef ptConnection) const

{
    for (PeerRef ptPeer : m_aptPeers)
    {
        if (ptPeer->m_ptConnection == ptConnection)
        {
            return ptPeer;
        }
    }

    return NULL;
}
} }


















































namespace Console { namespace Online {
#line 3809
PacketResult Party::ReceiveChunkedData(
    const bdConnectionRef ptConnection,
    const bdMessageRef ptMessage,
    PartyChunkedDataType & eChunkType,
    PartyPlayer *& ptPlayer,
    BOOL & bComplete)
{
    D3PartyMsgPlayerDataChunk tChunkMsg(0, CHUNKED_DATA_INVALID, 0);
    uint8 tBuffer[MAX_CHUNK_SIZE];
    uint32 uBufferSize = sizeof(tBuffer);

    if (!tChunkMsg.Deserialize(ptMessage->getPayload(), tBuffer, uBufferSize))
    {
        return PACKET_CORRUPT;
    }


    if (tChunkMsg.m_uUncompressedSize > MAX_CHUNKED_UNCOMPRESSED_SIZE)
    {
        return PACKET_CORRUPT;
    }


    eChunkType = tChunkMsg.m_eType;
    if (eChunkType == CHUNKED_DATA_INVALID || eChunkType >= CHUNKED_DATA_MAX)
    {
        return PACKET_CORRUPT;
    }



    PartyPlayer & tPlayer = m_tPlayers[tChunkMsg.m_idPlayer];


    if (!tPlayer.IsValid() ||
        (IsHost() ? *ptConnection->getAddress() != *tPlayer.m_ptAddr : ptConnection != m_ptHostConnection))
    {
        return PACKET_INVALID;
    }


    if (tPlayer.m_szChunkDataBuffer.empty())
    {
        tPlayer.m_eChunkType = eChunkType;
        tPlayer.m_uChunkUncompressedSize = tChunkMsg.m_uUncompressedSize;
    }
    else if (tPlayer.m_eChunkType != eChunkType)
    {
        return PACKET_INVALID;
    }


    if (tPlayer.m_szChunkDataBuffer.size() + uBufferSize > MAX_CHUNKED_BUFFER_SIZE)
    {
        return PACKET_CORRUPT;
    }


    tPlayer.m_szChunkDataBuffer.append(reinterpret_cast<const char *>(tBuffer), uBufferSize);

    ptPlayer = &tPlayer;
    bComplete = tChunkMsg.m_bComplete;

    return PACKET_OK;
}
} }
#line 4832

namespace Console { namespace Online {
// NON_MATCHING: 748 vs 744 bytes; original converts bHasVoicePrivs/bBadReputation (cset) once before the slot checks and keeps one slot-index register, ours converts after the bdReference copy and keeps two; tried continue-style guards, nested if with == FALSE/== 0, and !IsValid() && !(mask).
#line 1493
PartyPlayer * Party::AddPlayer(
    bdCommonAddrRef ptAddr,
    int32 nControllerIndex,
    const char * szPlayerName,
    const AdditionalPlatformInformation & tAdditionalPlatformInfo,
    OnlineUserId idOnlineUser,
    PlayerType ePlayerType,
    int32 nSponsorIndex,
    BOOL bHasVoicePrivs,
    BOOL bBadReputation)
#line 1503
{
    if (m_uNumPlayers >= m_uMaxPlayers)
        return NULL;
#line 1509
    for (uint8 i = 0; i < MAX_PEERS; ++i)
    {
        PartyPlayer & tPlayer = m_tPlayers[i];
#line 1513
        if (!tPlayer.IsValid()
#line 1515
            && !(m_fPlayersInGame & (1 << i)))
        {
#line 1521
            tPlayer.Set(i, ptAddr, nControllerIndex, szPlayerName,
                        tAdditionalPlatformInfo, idOnlineUser, ePlayerType,
                        nSponsorIndex, bHasVoicePrivs, bBadReputation);
#line 1533
            m_uNumPlayers++;
#line 1535
            MarkGameInfoDirty();
#line 1537
            QueueNotification<int32>(GetPartyMemberAddedEvent(), i);
            return &tPlayer;
        }
    }
    return NULL;
}
#line 1549
PartyPlayer * Party::AddPlayer(PartyPlayer & tPlayer)
{
#line 1552
    PartyPlayer * ptPlayer = AddPlayer(tPlayer.m_ptAddr, tPlayer.m_nControllerIndex,
        tPlayer.m_szName, tPlayer.m_tAdditionalPlatformInfo, tPlayer.m_idOnlineUser,
        tPlayer.m_ePlayerType, tPlayer.m_nSponsorIndex, tPlayer.m_bVoiceActive,
        tPlayer.m_bBadReputation);
    return ptPlayer;
}
#line 1568
PartyPlayer * Party::AddPlayer(
    bdCommonAddrRef ptAddr,
    D3PartyMsgAddPlayerRequest & tRequest)
{
    PartyPlayer * ptPlayer = AddPlayer(ptAddr,
        tRequest.m_nControllerIndex,
        tRequest.m_szPlayerName,
        tRequest.m_tAdditionalPlatformInfo,
        tRequest.m_idOnlineUser,
        static_cast<PlayerType>(tRequest.m_ePlayerType),
        tRequest.m_nGuestSponsor,
        tRequest.m_bVoiceActive,
        tRequest.m_bBadReputation);
    return ptPlayer;
}
#line 182
void PartyPlayer::StartReservation()
{
    m_uReservationTime = m_ptAddr->isLoopback() ? 0 : Blizzard::Time::Milliseconds();
}
#line 2316
JoinResult Party::onConnectRequest(D3PartyJoinRequest & tJoinRequest, D3PartyJoinResponse & tJoinResponse)
{
    if (!CheckVersionCompatability(tJoinRequest.m_uTitleVersion)) {
        if (GetTitleVersion() > tJoinRequest.m_uTitleVersion)
            return JOIN_FAILED_REQUIRES_UPDATE;
        return JOIN_FAILED_HOST_VERSION_TOO_OLD;
    }
    if (tJoinRequest.m_eJoinType == JOIN_TYPE_MATCHMAKE && (m_bIsPaused || m_bIsSoftPaused))
        return JOIN_FAILED_PAUSED_DURING_MATCHMAKE;
    if (tJoinRequest.m_bIsOnline != (IsOnline() != FALSE))
        return JOIN_FAILED;
#line 2373
    uint32 uNumReservedSlots = 0;
    for (uint32 uRequestPlayerIdx = 0; uRequestPlayerIdx < tJoinRequest.m_uNumPlayers; ++uRequestPlayerIdx) {
#line 2378
        PartyPlayer & tJoinPlayer = tJoinRequest.m_tPlayers[uRequestPlayerIdx];
#line 2380
        for (int32 nPlayerIdx = GetFirstPlayerIndex(); nPlayerIdx != -1; nPlayerIdx = GetNextPlayerIndex(nPlayerIdx)) {
#line 2384
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];
            if (tPlayer.IsSamePlayer(tJoinPlayer)) {
                if (tPlayer.IsReserved())
                    ++uNumReservedSlots;
                else
                    return JOIN_FAILED;
                break;
            }
        }
    }
    if (uNumReservedSlots == tJoinRequest.m_uNumPlayers)
        return JOIN_SUCCESS;
    if (m_eOnlineState == PARTY_ONLINE_INVITEONLY && tJoinRequest.m_eJoinType != JOIN_TYPE_INVITE)
        return JOIN_FAILED_INVITE_ONLY;
#line 2413
    uint32 uOccupiedSlots = 0;
    for (int32 i = 0; i < m_uMaxPlayers; ++i) {
#line 2416
        PartyPlayer & tPlayer = m_tPlayers[i];
        if (tPlayer.IsValid() || (m_fPlayersInGame & (1 << i)))
            ++uOccupiedSlots;
    }
#line 2424
    uint32 uNumRequiredSlots = tJoinRequest.m_uNumPlayers - uNumReservedSlots;
    if (uNumRequiredSlots + uOccupiedSlots > m_uMaxPlayers)
        return JOIN_FAILED_GAME_FULL;
#line 2430
    for (uint32 uRequestPlayerIdx = 0; uRequestPlayerIdx < tJoinRequest.m_uNumPlayers; ++uRequestPlayerIdx) {
#line 2434
        PartyPlayer & tJoinPlayer = tJoinRequest.m_tPlayers[uRequestPlayerIdx];
#line 2436
        BOOL bFound = FALSE;
        for (int32 nPlayerIdx = GetFirstPlayerIndex(); !bFound && nPlayerIdx != -1; nPlayerIdx = GetNextPlayerIndex(nPlayerIdx)) {
#line 2441
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];
            bFound = tPlayer.IsSamePlayer(tJoinPlayer);
        }
        if (!bFound) {
#line 2447
            PartyPlayer * ptNewPlayer = AddPlayer(tJoinPlayer);
            ptNewPlayer->StartReservation();
        }
    }
    return JOIN_SUCCESS;
}
} }
#line 4972

#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
namespace Console { namespace Online {
// Original data is -1; DWARF records the owner but no definition line.
int32 Peer::ms_idTopPeer = -1;
#line 4669
PeerRef Party::AddPeer(bdConnectionRef ptConnection, bdNATType eNATType)
{
#line 4673
    PeerRef ptPeer = ::new Peer(ptConnection, eNATType);
    m_aptPeers.push_back(ptPeer);
    ptConnection->registerListener(this);
    return ptPeer;
}
#line 3430
PacketResult Party::HandlePeerHello(const bdReceivedMessage & tReceivedMsg)
{
    if (m_eConnectionState != PARTY_CONNECTED)
        return PACKET_NOT_HANDLED;
#line 3438
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();
#line 3441
    bdNChar8 szAddress[22];
    bdAddressMap::addrToString(ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
#line 3444
    PeerRef ptPeer = FindPeer(ptConnection);
    if (ptPeer.notNull())
        return PACKET_OK;
#line 3451
    D3PartyMsgPeerHello tHelloMessage(bdSecurityID(), bdSecurityKey(), BD_NAT_UNKNOWN);
    if (!tHelloMessage.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;
    if (bdMemcmp(&tHelloMessage.m_tSecID, &m_tSecID, sizeof(m_tSecID)) != 0 ||
        bdMemcmp(&tHelloMessage.m_tSecKey, &m_tSecKey, sizeof(m_tSecKey)) != 0)
        return PACKET_CORRUPT;
    ptPeer = AddPeer(ptConnection, static_cast<bdNATType>(tHelloMessage.m_uNATType));
    if (VoiceChat::Get()) {
        UpdateVoiceChatSession(FALSE);
        VoiceChat::Get()->PeerConnected(ptPeer, FALSE);
    }
    return PACKET_OK;
}
} }
#line 5017

namespace Console { namespace Online {
#line 4381
void Party::FinalizeMigration()
{
    SetMigrationState(MIGRATION_FINALIZING);
#line 4387
    for (blz::vector<PeerRef>::iterator it = m_aptPeers.begin(); it != m_aptPeers.end();) {
#line 4389
        PeerRef ptPeer = *it;
        if (!ptPeer->m_bHasRequestedMigration) {
#line 4392
            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptPeer->m_ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
            it = m_aptPeers.erase(it);
            CloseConnection(ptPeer->m_ptConnection);
        } else {
            ptPeer->m_bHasRequestedMigration = FALSE;
            ++it;
        }
    }
    m_ptHostConnection = NULL;
#line 4407
    for (bdCommonAddrRef & ptMigratingPeerAddr : m_aptMigrationInfo) {
        if (ptMigratingPeerAddr->isLoopback())
            break;
#line 4415
        PeerRef ptPeer = FindPeer(ptMigratingPeerAddr);
        if (ptPeer) {
#line 4418
            bdConnectionRef ptPeerConnection = ptPeer->m_ptConnection;
            bdNChar8 szAddress[22];
            bdAddressMap::addrToString(ptPeerConnection->getAddressHandle(), szAddress, sizeof(szAddress));
            return;
        }
    }
    BecomeHost();
}
} }
#line 5057

namespace Console { namespace Online {
#line 3541
PacketResult Party::HandleAddPlayerResponse(const bdReceivedMessage & tReceivedMsg)
{
    if (IsHost())
        return PACKET_INVALID;
#line 3549
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();
    const bdMessageRef ptMessage = tReceivedMsg.getMessage();
#line 3552
    D3PartyMsgAddPlayerResponse tResponse(-1, -1);
    if (!tResponse.Deserialize(ptMessage->getPayload()))
        return PACKET_CORRUPT;
    if (tResponse.m_idPlayer == -1) {
        if (m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex]) {
            QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyAddPlayerCallback>, int>::bind<&OnlineService::internal::callback_base<1, PartyAddPlayerCallback>::execute>(m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex], FALSE));
            m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex] = PartyAddPlayerCallback();
        }
        return PACKET_OK;
    }
    if (!GamerProfile::IsHeroLoaded(tResponse.m_nControllerIndex)) {
        m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex] = PartyAddPlayerCallback();
        PartyRemovePlayer(tResponse.m_nControllerIndex);
        return PACKET_OK;
    }
#line 3578
    PartyPlayer & tPlayer = m_tPlayers[static_cast<uint32>(tResponse.m_idPlayer)];
    tPlayer.UpdateAccountAndHero();
    SendPlayerAccountAndHeroToHost(tPlayer, SEND_ACCOUNT_AND_HERO);
    if (m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex]) {
        QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyAddPlayerCallback>, int>::bind<&OnlineService::internal::callback_base<1, PartyAddPlayerCallback>::execute>(m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex], TRUE));
        m_apfnAddPlayerCallbacks[tResponse.m_nControllerIndex] = PartyAddPlayerCallback();
    }
    return PACKET_OK;
}
} }
#line 5095

namespace Console { namespace Online {
#line 4114
void Party::UpdateVoiceChatSession(
    BOOL bIsOnPartyJoined)
{
    VoiceChat::PartyMember atPartyMembers[MAX_PEERS];
    int32 nCountMembers = 0;

#line 4121
    for (int32 nPlayerIdx = GetFirstPlayerIndex(); nPlayerIdx != -1;
         nPlayerIdx = GetNextPlayerIndex(nPlayerIdx)) {
#line 4125
        PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];
        if (tPlayer.IsReserved() ||
            (tPlayer.IsLocal() && tPlayer.m_bIsLeaving) ||
            (!tPlayer.m_bVoiceActive && IsOnline()))
            continue;
#line 4132
        VoiceChat::PartyMember & member = atPartyMembers[nCountMembers++];
        member = VoiceChat::PartyMember();
        member.m_idOnlineUser = tPlayer.m_idOnlineUser;
        member.m_ptAddr = tPlayer.m_ptAddr;
        member.m_bIsLocal = tPlayer.IsLocal();
        member.m_nLocalControllerIndex = tPlayer.m_nControllerIndex;
        member.m_bIsPrimary = tPlayer.IsPrimary();
        member.m_nPlayerIndex = nPlayerIdx;

        PeerRef ptPeer = FindPeer(tPlayer.m_ptAddr);
        member.m_idPeer = ptPeer ? ptPeer->m_idPeer : -1;
    }
    if (VoiceChat::Get()) {
        VoiceChat::Get()->UpdatePartyMembership(atPartyMembers, nCountMembers, bIsOnPartyJoined);
        VoiceChat::Get()->UpdatePeerList(m_aptPeers);
    }
}
} }
#line 5133

namespace Console { namespace Online {
// NON_MATCHING: same size, 4 instructions; original computes wasInAdHocMode (load of m_eOnlineState) before willBeInAdHocMode, ours the reverse; tried both != operand orders and eState vs m_ePendingOnlineState (col 52 at 618 favours the latter).
#line 574
void Party::SetOnlineState(
    PartyOnlineState eState)
{
    if (m_ePendingOnlineState == eState)
        return;
    if (eState == PARTY_OFFLINE || eState == PARTY_ADHOC ||
        (eState == PARTY_LAN && IsOnline()) || m_eOnlineState == PARTY_LAN) {
#line 593
        for (int32 nPlayerIdx = GetFirstPlayerIndex(); nPlayerIdx != -1;
             nPlayerIdx = GetNextPlayerIndex(nPlayerIdx)) {
#line 597
            PartyPlayer & tPlayer = m_tPlayers[nPlayerIdx];
            if (!tPlayer.IsLocal())
                RemovePlayer(nPlayerIdx);
        }
#line 605
        for (PeerRef ptPeer : m_aptPeers) {
            CloseConnection(ptPeer->m_ptConnection);
        }
        m_aptPeers.clear();
    }
    m_ePendingOnlineState = eState;
    SetPreferredOnlineState(eState);
#line 617
    bool wasInAdHocMode = GetOnlineState() == PARTY_ADHOC;
    bool willBeInAdHocMode = m_ePendingOnlineState == PARTY_ADHOC;
    if (wasInAdHocMode != willBeInAdHocMode) {
        if (eState == PARTY_ADHOC)
            SwitchToNetworkMode(NET_ADHOC, TRUE);
        else
            SwitchToNetworkMode(NET_OFFLINE, TRUE);
    } else if (eState == PARTY_OFFLINE || (eState == PARTY_LAN && IsNetworkRunning())) {
        FinalizeOnlineStateChange();
    } else {
        if (m_ePendingOnlineState != PARTY_LAN)
            SwitchToNetworkMode(NET_ONLINE, FALSE);
        if (!IsNetworkRunning())
            m_eOnlineState = PARTY_OFFLINE;
    }
}
} }
#line 5179

#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
namespace Console { namespace Online {
#line 1258
void Party::Join(
    PartyPendingJoinInfo & tJoinInfo)
{
    if (tJoinInfo.m_ptAddress.isNull() || tJoinInfo.m_ptAddress->isLoopback()) {
        onPartyJoinResult(JOIN_FAILED_UNABLE_TO_CONNECT);
        return;
    }
    m_pfnJoinCallback = tJoinInfo.m_pfnCallback;
    m_tSecID = tJoinInfo.m_tSecID;
    m_tSecKey = tJoinInfo.m_tSecKey;
    m_bRegisteredKey = bdSingleton<bdNetImpl>::getInstance()->getSocketRouter()->getKeyMap()->registerKey(m_tSecID, m_tSecKey);
    if (!m_bRegisteredKey) {
        if (!sptParty || !(sptParty->m_tSecID == m_tSecID) || !(sptParty->m_tSecKey == m_tSecKey)) {
            onPartyJoinResult(JOIN_FAILED);
            return;
        }
    }
    m_ptHostConnection = InternalConnect(tJoinInfo.m_ptAddress, m_tSecID);
    if (!m_ptHostConnection) {
        onPartyJoinResult(JOIN_FAILED_UNABLE_TO_CONNECT);
        return;
    }
    AddPeer(m_ptHostConnection, BD_NAT_UNKNOWN);
#line 1305
    D3PartyJoinRequest tJoinRequest(tJoinInfo.m_bIsOnline, GetNATType(), tJoinInfo.m_eJoinType);
    for (int32 i = sptParty->GetFirstPlayerIndex(); i != -1; i = sptParty->GetNextPlayerIndex(i)) {
#line 1308
        PartyPlayer & tPlayer = sptParty->m_tPlayers[i];
        if (tJoinInfo.m_bIsOnline != sptParty->IsOnline() && !tPlayer.IsLocal())
            continue;
#line 1317
        PartyPlayer & tAddedPlayer = tJoinRequest.AddPlayer(tPlayer);
        if (tJoinInfo.m_bIsOnline && tPlayer.IsLocal()) {
#line 1334
            nn::account::NetworkServiceAccountId idAddedPlayerNSA;
            BOOL bGotNSA = GamerProfile::GetUserNetworkServiceAccountId(tAddedPlayer.m_nControllerIndex, idAddedPlayerNSA);
            tAddedPlayer.m_idOnlineUser = bGotNSA ? idAddedPlayerNSA.id : 0;
        }
    }
    m_ptJoinRequestMessage = ::new bdMessage(D3_MSG_PARTY_JOIN_REQ, false);
    tJoinRequest.Serialize(m_ptJoinRequestMessage->getPayload());
    m_eConnectionState = PARTY_CONNECTING;
}
} }
#line 5228

namespace Console { namespace Online {
#line 439
Party::Party(int32 nControllerIndex)
    : m_eConnectionState(PARTY_NOT_CONNECTED),
      m_ptSessionManager(NULL),
      m_bOwnsPresenceSessionKey(FALSE),
      m_bRegisteredKey(FALSE),
      m_bIsHost(FALSE),
      m_idHostPlayer(-1),
      m_bGameInfoDirty(FALSE),
      m_uLastGameInfoUpdateTime(0),
      m_bGameInProgress(FALSE),
      m_bInGame(FALSE),
      m_bIsPaused(FALSE),
      m_bIsSoftPaused(FALSE),
      m_bMatchmakingEnabled(FALSE),
      m_eOnlineState(PARTY_OFFLINE),
      m_ePendingOnlineState(PARTY_OFFLINE),
      m_ePreferredOnlineState(PARTY_ONLINE_FRIENDSONLY),
      m_nControllerIndex(nControllerIndex),
      m_uMaxPlayers(MAX_PEERS),
      m_uNumPlayers(0),
      m_fPlayersInGame(0),
      m_eMigrationState(MIGRATION_IDLE),
      m_uMigrationStateStart(0),
      m_bQoSListening(FALSE)
{
#line 467
    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    const bdCommonAddrRef ptLocalAddr = ptNet->getLocalCommonAddr();
    ptNet->getConnectionStore()->registerDispatchInterceptor(this);
    sgtParties.push_back(this);
}
} }
#line 5264

namespace Console { namespace Online {
// NON_MATCHING: 1536 vs 1528 bytes; the bOk / m_uResult tests fold into one and+b.ne where the original keeps cbnz + eor/tbnz; tried bOk && result, result && bOk, nested ifs with bCleanup = !bOk.
#line 3333
PacketResult Party::HandleJoinResponse(
    const bdReceivedMessage & tReceivedMsg)
{
    const bdConnectionRef ptConnection = tReceivedMsg.getConnection();

    if (m_eConnectionState != PARTY_NEGOTIATING ||
        ptConnection != m_ptHostConnection)
    {
        return PACKET_NOT_HANDLED;
    }

    const bdMessageRef ptMessage = tReceivedMsg.getMessage();
    const bdBitBufferRef ptBuffer = ptMessage->getPayload();

    D3PartyJoinResponse tJoinResponse(JOIN_SUCCESS);
    BOOL bOk = tJoinResponse.Deserialize(ptBuffer);
    // Inferred: no row; cleared once the join completes.
    BOOL bCleanup = TRUE;


    if (bOk)
    {
        if (tJoinResponse.m_uResult == JOIN_SUCCESS)
        {
#line 3361
            m_eConnectionState = PARTY_CONNECTED;

            if (onPartyJoined(ptBuffer))
            {

                for (D3PartyJoinResponse::Peer & tJoinPeer : tJoinResponse.m_atPeers)
                {
                    bdCommonAddrRef ptPeerAddress = tJoinPeer.ptAddr;
                    bdNATType ePeerNATType = static_cast<bdNATType>(tJoinPeer.uNATType);

                    if (!ptPeerAddress->isLoopback())
                    {

                        bdConnectionRef ptPeerConnection = InternalConnect(ptPeerAddress, m_tSecID);
                        if (ptPeerConnection)
                        {
                            AddPeer(ptPeerConnection, ePeerNATType);
                        }
#line 3383
                    }
                }

                onConnect(m_ptHostConnection);
                // Inferred: no row of its own.
                bCleanup = FALSE;
            }
            else
            {

                if (sptParty == this)
                {
                    sptParty = NULL;
                }
            }
        }
    }
#line 3412
    if (bCleanup)
    {
        onPartyJoinResult(static_cast<JoinResult>(tJoinResponse.m_uResult));

        m_eConnectionState = PARTY_NOT_CONNECTED;
        Leave();
    }
    else
    {
        onPartyJoinResult(JOIN_SUCCESS);
    }

    return PACKET_OK;
}
} }
#line 5346

namespace Console { namespace Online {
#line 2048
bdBool Party::Deserialize(
    bdBitBufferRef ptBitBuffer)
{
#line 2053
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
#line 2055
    uint8 uNumPlayers = 0;
    uint8 uMaxPlayers = 0;
    bdUByte8 eOnlineState = 0;
    bdBool bGameInProgress = false;
    bdBool bIsPaused = false;
    bdBool bIsSoftPaused = false;
#line 2062
    bdBool bOk = ptBitBuffer->readUByte8(uNumPlayers);
    bOk = bOk && ptBitBuffer->readUByte8(uMaxPlayers);
    bOk = bOk && ptBitBuffer->readUByte8(eOnlineState);
    bOk = bOk && eOnlineState > PARTY_OFFLINE && eOnlineState <= NUM_PARTY_ONLINE_STATES;
    bOk = bOk && ptBitBuffer->readBool(bGameInProgress);
    bOk = bOk && ptBitBuffer->readBool(bIsPaused);
    bOk = bOk && ptBitBuffer->readBool(bIsSoftPaused);
    bOk = bOk && ptBitBuffer->readByte8(m_idHostPlayer);
    bOk = bOk && m_idHostPlayer != -1;
    if (!bOk)
        return false;
    m_uNumPlayers = uNumPlayers;
    m_uMaxPlayers = uMaxPlayers;
    m_eOnlineState = m_ePendingOnlineState = eOnlineState;
    m_bGameInProgress = bGameInProgress;
    m_bIsPaused = bIsPaused;
    m_bIsSoftPaused = bIsSoftPaused;
    bOk = m_tGameParams.Deserialize(ptBitBuffer);
    if (m_tGameParams.m_eGameType != GT_CHALLENGE_RIFTS &&
        (m_tGameParams.m_tWeeklyChallengeConfig.has_challenge_number() ||
         m_tGameParams.m_tWeeklyChallengeData.has_hero_snapshot())) {
        m_tGameParams.m_tWeeklyChallengeConfig.Clear();
        m_tGameParams.m_tWeeklyChallengeData.Clear();
    }
#line 2119
    for (int32 i = 0; i < MAX_PEERS; ++i) {
#line 2121
        PartyPlayer & tPlayer = m_tPlayers[i];
#line 2123
        bdBool bIsValid = false;
        bOk = bOk && ptBitBuffer->readBool(bIsValid);
        if (bOk) {
            if (bIsValid) {
                bOk = tPlayer.Deserialize(me, ptBitBuffer);
                if (bOk)
                    tPlayer.m_idPlayer = i;
            } else if (tPlayer.IsValid()) {
                tPlayer.Clear();
            }
        }
    }
    MarkGameInfoDirty();
#line 2146
    uint32 uNumMigrationInfo = 0;
    bOk = bOk && ptBitBuffer->readRangedUInt32(uNumMigrationInfo, 0, MAX_PEERS, true);
    m_aptMigrationInfo.clear();
    for (uint32 i = 0; bOk && i < uNumMigrationInfo; ++i) {
#line 2151
        bdCommonAddrRef ptMigrationAddr = ::new bdCommonAddr;
        bOk = ptMigrationAddr->deserialize(me, ptBitBuffer);
        if (bOk)
            m_aptMigrationInfo.push_back(ptMigrationAddr);
    }
    return bOk;
}
} }
#line 5419
