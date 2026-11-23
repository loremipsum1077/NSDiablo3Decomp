// Original file: X/Client/Console/VoiceChat.cpp
// VoiceChat::Get() returns NULL in this build, so the callers' uses of the VoiceChat object fold
// away; only the code that survives in the binary is written for those functions.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "VoiceChat.h"
#include "ConsoleParty.h"
#include "Online.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"

namespace Console {
namespace Online {

extern PartyRef sptParty;  // ConsoleParty.cpp
typedef int32 PartyMuteState;  // Online.h:237; NOT_MUTED = 0



BOOL IsVoiceChatEnabled()
{





    return FALSE;
}



BOOL IsVoiceChatRestricted()
{
    if (!PartyIsOnline())
    {
        return FALSE;
    }






    return FALSE;
}


































PartyMuteState IsPlayerMuted(int32 nTargetPartyMemberIndex)
{



























    return 0;
}



void SetPlayerMuted(int32 nControllerIndexInitatedTheMuting, int32 nPartyMemberIndexToBeMuted, BOOL bMuted)
{



    PartyRef ptParty = sptParty;









}



BOOL IsPartyMemberMicConnected(int32 nPartyMemberIndex)
{







    return FALSE;
}


























































































































void SendVoiceChatMicDetected(bdBitBuffer & tPayload, PeerRef ptTargetPeer)


{
    ScopedNetworkThreadLock tLock;

    if (!sptParty ||
        sptParty->m_aptPeers.empty())
    {
        return;
    }

    bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_VOICECHAT_MICDETECTED, false);
    ptMessage->getPayload()->append(tPayload);

    ScopedNetworkSendLock tSendLock;
    if (ptTargetPeer.isNull())
    {
        for (PeerRef & ptPeer : sptParty->m_aptPeers)
        {


            ptPeer->m_ptConnection->send(ptMessage, true);
        }
    }
    else
    {


        ptTargetPeer->m_ptConnection->send(ptMessage, true);
    }
}
}  // namespace Online

void VoiceChat::ReceiveRemoteVoiceChatData(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection)
{







}


void VoiceChat::ReceiveRemoteVoiceChatReliableData(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection)
{







}


void VoiceChat::ReceiveVoiceChatMuteUpdate(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection)
{







}


void VoiceChat::ReceiveVoiceChatMicDetected(int32 idPeer, bdMessageRef ptMessage, bdConnectionRef ptConnection)
{







}
















































































































































BOOL VoiceChat::GetLocalPlayerIndex(int32 nLocalControllerIndex, int32 & outPlayerIndex) const
{


    for (uint32 i = 0; i < m_uCountMembers; i++)
    {

        if (m_members[i].m_nLocalControllerIndex == nLocalControllerIndex &&
            m_members[i].m_bIsLocal)
        {
            outPlayerIndex = m_members[i].m_nPlayerIndex;
            return TRUE;
        }
    }

    return FALSE;
}





































































































BOOL VoiceChat::IsPlayerMicConnected(int32 nPlayerIndex)
{


    if (IsMutedCommunication(nPlayerIndex, -1))
    {
        return FALSE;
    }

    int32 nArrayIndex = (nPlayerIndex == -1) ? 4 : nPlayerIndex;
    return (m_uPartyMicConnectedFlags & (1 << nArrayIndex)) != 0;
}






void VoiceChat::SetPlayerMicConnected(int32 nPlayerIndex, BOOL isMicConnected, BOOL sendUpdateToPeers, Online::PeerRef ptPeer)




{
    int32 nArrayIndex = (nPlayerIndex == -1) ? 4 : nPlayerIndex;

    bool isChanged = IsPlayerMicConnected(nPlayerIndex) != isMicConnected;








    if (isMicConnected) { m_uPartyMicConnectedFlags |= (1 << nArrayIndex); } else { m_uPartyMicConnectedFlags &= ~(1 << nArrayIndex); }


    if (nPlayerIndex != -1 && sendUpdateToPeers)
    {

        QueueFunction(OnlineService::internal::function_binder_3<void, int32, BOOL, Online::PeerRef>::bind<&VoiceChat::SetPlayerMicConnected_SendToPeers>(nPlayerIndex, isMicConnected, ptPeer));
    }






}


void VoiceChat::SetPlayerMicConnected_SendToPeers(int32 nPlayerIndex, BOOL isMicConnected, Online::PeerRef ptPeer)



{
    bdBitBuffer tPayload(0, false);
    tPayload.writeInt32(nPlayerIndex);
    tPayload.writeBool(isMicConnected);

    Online::SendVoiceChatMicDetected(tPayload, ptPeer);
}
































template <typename T>
static uint32 ComputeDifference(T * pOld, uint32 uNumOld, T * pNew, uint32 uNumNew, T * pDiff, bool fGetAdded)
{
    uint32 uNumLeft;
    uint32 uNumRight;
    uint32 uNumDiff = 0;
    T * pRight;
    T * pLeft;
    if (fGetAdded) {
        pLeft = pNew;
        uNumLeft = uNumNew;
        pRight = pOld;
        uNumRight = uNumOld;
    }
    else
    {
        pLeft = pOld;
        uNumLeft = uNumOld;
        pRight = pNew;
        uNumRight = uNumNew;
    }
    for (uint32 n = 0; n < uNumLeft; n++)
    {
        BOOL bFound = FALSE;
        for (uint32 o = 0; !bFound && o < uNumRight; o++)
        {
            bFound = (pRight[o] == pLeft[n]);
        }
        if (!bFound)
        {
            pDiff[uNumDiff++] = pLeft[n];
        }
    }
    return uNumDiff;
}




void VoiceChat::UpdatePartyMembership(const PartyMember * pPartyMembers, int32 nNumPartyMembers, BOOL bIsOnPartyJoined)



{
    m_isOnline = Online::PartyIsOnline();

    if (bIsOnPartyJoined)
    {



        UpdatePartyMembership(NULL, 0, FALSE);



        m_uPartyMicConnectedFlags = 0;
    }

    int32 currentLocals[4];
    int32 currentRemotes[4];
    int32 deltaLocals[4];
    int32 deltaRemotes[4];
    uint32 uNumCurrentLocals = 0;
    uint32 uNumCurrentRemotes = 0;







    m_uCountMembers = 0;
    for (int32 i = 0; i < nNumPartyMembers; i++)
    {
        const PartyMember & tPlayer = pPartyMembers[i];
        m_members[m_uCountMembers++] = tPlayer;
        if (tPlayer.m_bIsLocal)
        {
            currentLocals[uNumCurrentLocals++] = tPlayer.m_nLocalControllerIndex;
        }
        else
        {
            currentRemotes[uNumCurrentRemotes++] = tPlayer.m_nPlayerIndex;
        }
    }


    uint32 uCountChanges = 0; uint32 uNumDeltaPlayers = ComputeDifference(m_anLocalPlayerControllerIndices, m_uNumLocalPlayers,
                                         currentLocals, uNumCurrentLocals, deltaLocals, false);


    uCountChanges += uNumDeltaPlayers;
    if (uNumDeltaPlayers > 0)
    {
        for (uint32 i = 0; i < uNumDeltaPlayers; i++)
        {
            int32 nPlayerIndex;
            if (GetLocalPlayerIndex(deltaLocals[i], nPlayerIndex))
            {

                SetPlayerMicConnected(nPlayerIndex, FALSE, FALSE, Online::PeerRef());
            }
        }
    }




    LocalPlayersRemoved(uNumDeltaPlayers, deltaLocals);


    uNumDeltaPlayers = ComputeDifference(m_anLocalPlayerControllerIndices, m_uNumLocalPlayers,
                                         currentLocals, uNumCurrentLocals, deltaLocals, true);


    uCountChanges += uNumDeltaPlayers;
    if (uNumDeltaPlayers > 0)
    {
        for (uint32 i = 0; i < uNumDeltaPlayers; i++)
        {
            int32 nPlayerIndex;
            if (GetLocalPlayerIndex(deltaLocals[i], nPlayerIndex))
            {

                SetPlayerMicConnected(nPlayerIndex, FALSE, FALSE, Online::PeerRef());
            }
        }
    }




    LocalPlayersAdded(uNumDeltaPlayers, deltaLocals);


    for (uint32 i = 0; i < uNumCurrentLocals; i++)
    {
        m_anLocalPlayerControllerIndices[i] = currentLocals[i];
    }
    m_uNumLocalPlayers = uNumCurrentLocals;




    uNumDeltaPlayers = ComputeDifference(m_anRemotePlayerIndices, m_uNumRemotePlayers,
                                         currentRemotes, uNumCurrentRemotes, deltaRemotes, false);


    uCountChanges += uNumDeltaPlayers;
    if (uNumDeltaPlayers > 0)
    {
        for (uint32 i = 0; i < uNumDeltaPlayers; i++)
        {
            int32 nPlayerIndex = deltaRemotes[i];

            SetPlayerMicConnected(nPlayerIndex, FALSE, FALSE, Online::PeerRef());
        }
    }

    RemotePlayersRemoved(uNumDeltaPlayers, deltaRemotes);


    uNumDeltaPlayers = ComputeDifference(m_anRemotePlayerIndices, m_uNumRemotePlayers,
                                         currentRemotes, uNumCurrentRemotes, deltaRemotes, true);
    uint32 uNumRemotePlayersAdded = uNumDeltaPlayers;

    uCountChanges += uNumDeltaPlayers;
    if (uNumDeltaPlayers > 0)
    {
        for (uint32 i = 0; i < uNumDeltaPlayers; i++)
        {
            int32 nPlayerIndex = deltaRemotes[i];



            SetPlayerMicConnected(nPlayerIndex, FALSE, FALSE, Online::PeerRef());



            for (uint8 uLocalControllerIndex = 0; uLocalControllerIndex < 4; uLocalControllerIndex++) { IsPlayerMuted(nPlayerIndex, uLocalControllerIndex); }
        }
    }







    RemotePlayersAdded(uNumDeltaPlayers, deltaRemotes);


    for (uint32 i = 0; i < uNumCurrentRemotes; i++)
    {
        m_anRemotePlayerIndices[i] = currentRemotes[i];
    }
    m_uNumRemotePlayers = uNumCurrentRemotes;

    if (uCountChanges != 0)
    {
        PartyMembershipUpdated();
    }

    if (bIsOnPartyJoined)
    {
        PartyJoined();
    }

    if (bIsOnPartyJoined || uNumRemotePlayersAdded > 0)
    {
        RecheckMicConnectedStatus(true, Online::PeerRef());
    }
}


void VoiceChat::OnOnlineStateChanged()
{
    m_isOnline = Online::PartyIsOnline();
    RecheckOnlineState(TRUE);
}


void VoiceChat::QueueFunction(const VoiceChatFunctionRunOnThread & cb)
{

    Blizzard::Lock::ScopedMutex tLock(m_queueLockThread);
    m_queueVoiceChatThread.enqueue(cb);
}









































































VoiceChat * VoiceChat::Get()
{
    return NULL;
}

}  // namespace Console
