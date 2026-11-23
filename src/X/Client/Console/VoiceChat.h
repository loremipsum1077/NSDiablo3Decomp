// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/VoiceChat.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnection.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnectionListener.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdQueue.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "ConsoleParty.h"
#include "Online.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"

class bdBitBuffer;

namespace Console {
namespace Online {
typedef bdReference<Console::Online::Peer> PeerRef;

}  // namespace Online
}  // namespace Console
namespace Console {
class VoiceChat {  // size 0x118
    // vtable pointer
public:
    static void InitializeForApp();
    static void DisposeForApp();
    static Console::VoiceChat * Get();
    static void ReceiveRemoteVoiceChatData(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection);
    static void ReceiveRemoteVoiceChatReliableData(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection);
    static void ReceiveVoiceChatMuteUpdate(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection);
    static void ReceiveVoiceChatMicDetected(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection);
    struct PartyMember {  // size 0x28
        OnlineUserId m_idOnlineUser;  // 0x00
        bdCommonAddrRef m_ptAddr;  // 0x08
        BOOL m_bIsLocal;  // 0x10
        int32 m_nLocalControllerIndex;  // 0x14
        BOOL m_bIsPrimary;  // 0x18
        int32 m_nPlayerIndex;  // 0x1c
        int32 m_idPeer;  // 0x20
        PartyMember();
    };
    virtual void Init() = 0;  // vtable slot 0
    void Idle();
    virtual void PeerConnected(Console::Online::PeerRef, BOOL);  // vtable slot 1
    void UpdatePartyMembership(const Console::VoiceChat::PartyMember * pPartyMembers, int32 nNumPartyMembers, BOOL bIsOnPartyJoined);
    virtual void UpdatePeerList(const blz::vector<bdReference<Console::Online::Peer>, blz::allocator<bdReference<Console::Online::Peer> > > &);  // vtable slot 2
    void OnOnlineStateChanged();
    virtual BOOL IsVoiceChatRestricted() const = 0;  // vtable slot 3
    virtual BOOL IsVoiceChatActive() const = 0;  // vtable slot 4
    virtual BOOL IsPlayerMuted(int32, int32) = 0;  // vtable slot 5
    void SetPlayerMuted(int32, int32, BOOL);
    BOOL IsPlayerMicConnected(int32 nPlayerIndex);
    virtual void MuteListUpdatedExternallyEvent();  // vtable slot 6
protected:
    BOOL IsLocalPlayer(int32, int32 *) const;
    BOOL IsRemotePlayer(int32, OnlineUserId *) const;
    int32 GetLocalPrimaryPlayerIndex() const;
    BOOL GetPartyMember(int32, const Console::VoiceChat::PartyMember *&) const;
    BOOL GetOnlineUserId(bdConnectionRef, int32, OnlineUserId &, int32 &) const;
    BOOL GetOnlineUserId(int32, OnlineUserId &) const;
    BOOL GetPeerId(int32, int32 &) const;
    BOOL IsPrimary(int32) const;
    BOOL GetLocalPlayerControllerIndex(int32, int32 &) const;
    BOOL GetPlayerInfo(int32, bdCommonAddrRef &, int32 &, OnlineUserId &, BOOL &, BOOL &) const;
    BOOL GetLocalPlayerIndex(int32 nLocalControllerIndex, int32 & outPlayerIndex) const;
    typedef OnlineService::Callback<void ()> VoiceChatFunctionRunOnThread;
    void QueueFunction(const Console::VoiceChat::VoiceChatFunctionRunOnThread & cb);
    void PartyPushVoiceChatData(bdBitBuffer &, void *, uint32);
    virtual void Idle_Internal();  // vtable slot 7
    virtual void SetPlayerMuted_Internal(int32, int32, BOOL) = 0;  // vtable slot 8
    virtual bool IsMutedCommunication(int32, int32);  // vtable slot 9
    void SetPlayerMicConnected(int32 nPlayerIndex, BOOL isMicConnected, BOOL sendUpdateToPeers, Console::Online::PeerRef ptPeer);
    static void SetPlayerMicConnected_SendToPeers(int32 nPlayerIndex, BOOL isMicConnected, Console::Online::PeerRef ptPeer);
    virtual void RecheckMicConnectedStatus(bool, Console::Online::PeerRef) = 0;  // vtable slot 10
    virtual void LocalPlayersAdded(uint32, int32 *);  // vtable slot 11
    virtual void LocalPlayersRemoved(uint32, int32 *);  // vtable slot 12
    virtual void RemotePlayersAdded(uint32, int32 *);  // vtable slot 13
    virtual void RemotePlayersRemoved(uint32, int32 *);  // vtable slot 14
    virtual void PartyJoined();  // vtable slot 15
    virtual void PartyMembershipUpdated();  // vtable slot 16
    virtual void RecheckOnlineState(BOOL);  // vtable slot 17
    virtual void ProcessRemoteVoiceChatData(int32, const bdMessageRef &, const bdConnectionRef &);  // vtable slot 18
    virtual void ProcessRemoteVoiceChatReliableData(int32, const bdMessageRef &, const bdConnectionRef &);  // vtable slot 19
    virtual void ProcessVoiceChatMuteUpdate(int32, const bdMessageRef &, const bdConnectionRef &);  // vtable slot 20
    void ProcessRemoteVoiceChatMicDetected(int32, const bdMessageRef &, const bdConnectionRef &);
private:
    void SetPlayerMuted_OnThread(int32 nControllerIndexInitatedTheMuting, int32 nPlayerIndexToBeMuted, BOOL bMuted);
protected:
    VoiceChat();
    BOOL m_isOnline;  // 0x08
    Console::VoiceChat::PartyMember m_members[4];  // 0x10
    uint32 m_uCountMembers;  // 0xb0
    int32 m_anLocalPlayerControllerIndices[4];  // 0xb4
    uint32 m_uNumLocalPlayers;  // 0xc4
    int32 m_anRemotePlayerIndices[4];  // 0xc8
    uint32 m_uNumRemotePlayers;  // 0xd8
    uint32 m_uPartyMicConnectedFlags;  // 0xdc
    bool SetRemoteLastSpokeToNow(int32);
    void ResetRemoteLastSpoke(int32);
private:
    bdQueue<OnlineService::Callback<void ()> > m_queueVoiceChatThread;  // 0xe0
    Blizzard::Lock::RMutex m_queueLockThread;  // 0xf8
};
static_assert(sizeof(VoiceChat) == 0x118, "VoiceChat");

}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    57  Console::VoiceChat::PartyMember & Console::VoiceChat::PartyMember::operator=(Console::VoiceChat::PartyMember &&)  [inlined into 2 functions]
//   line    57  Console::VoiceChat::PartyMember::~PartyMember()  [inlined into 1 functions]
//   line    66  Console::VoiceChat::PartyMember::PartyMember()  [inlined into 1 functions]

#line 66
inline Console::VoiceChat::PartyMember::PartyMember()
    : m_idOnlineUser(0),
      m_ptAddr(),
      m_bIsLocal(FALSE),
      m_nLocalControllerIndex(-1),
      m_bIsPrimary(FALSE),
      m_nPlayerIndex(-1),
      m_idPeer(-1)
{
}
#line 130
