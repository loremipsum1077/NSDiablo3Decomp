#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "ConsoleParty.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdNet.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"

namespace Console {
namespace Online {
// Defined in X/Client/Console/Online.cpp.
uint64 GetTitleVersion();
BOOL CheckVersionCompatability(uint64 uVersion);
bdNATType GetNATType();





D3PartyJoinRequest::D3PartyJoinRequest(bdBool isOnline, bdNATType uNATType, JoinType eJoinType)
    : m_uTitleVersion(GetTitleVersion()),
      m_uNumPlayers(0),
      m_uNATType(uNATType),
      m_eJoinType(eJoinType),
      m_bIsOnline(isOnline)
{
}




PartyPlayer & D3PartyJoinRequest::AddPlayer(PartyPlayer & tAddPlayer)
{
    PartyPlayer & tPlayer = m_tPlayers[m_uNumPlayers++];
    tPlayer.Set(0, tAddPlayer.m_ptAddr, tAddPlayer.m_nControllerIndex, tAddPlayer.m_szName,
                tAddPlayer.m_tAdditionalPlatformInfo, tAddPlayer.m_idOnlineUser, tAddPlayer.m_ePlayerType,
                tAddPlayer.m_nSponsorIndex, tAddPlayer.m_bVoiceActive, tAddPlayer.m_bBadReputation);
    return tPlayer;
}









void D3PartyJoinRequest::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeUInt64(m_uTitleVersion);
    ptBitBuffer->writeUByte8(m_uNumPlayers);
    for (uint32 i = 0; i < m_uNumPlayers; i++)
    {
        m_tPlayers[i].Serialize(ptBitBuffer);
    }
    ptBitBuffer->writeRangedUInt32(m_uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
    ptBitBuffer->writeRangedUInt32(m_eJoinType, 0, 3, true);
    ptBitBuffer->writeBool(m_bIsOnline);
}





bdBool D3PartyJoinRequest::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    bdBool bOk = ptBitBuffer->readUInt64(m_uTitleVersion);
    if (!bOk)
    {
        return false;
    }

    // An incompatible version is answered with a rejection, not dropped as corrupt.
    if (!CheckVersionCompatability(m_uTitleVersion))
    {
        return true;
    }

    bOk = ptBitBuffer->readUByte8(m_uNumPlayers) && m_uNumPlayers <= MAX_PEERS;
    if (bOk)
    {
        for (uint32 i = 0; i < m_uNumPlayers; i++)
        {
            m_tPlayers[i].Deserialize(me, ptBitBuffer);
        }

        bOk = ptBitBuffer->readRangedUInt32(m_uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true) &&
              ptBitBuffer->readRangedUInt32(m_eJoinType, 0, 3, true) &&
              ptBitBuffer->readBool(m_bIsOnline);
    }

    return bOk;
}


D3PartyJoinResponse::D3PartyJoinResponse(bdUInt32 uResult)
    : m_uResult(JOIN_SUCCESS)
{
}


void D3PartyJoinResponse::AddPeers(const blz::vector<PeerRef> & aptPeers)
{
    for (PeerRef ptPartyPeer : aptPeers)
    {
        Peer & tPeer = m_atPeers.push_back();
        tPeer.ptAddr = ptPartyPeer->m_ptConnection->getAddress();
        tPeer.uNATType = ptPartyPeer->m_eNATType;
    }
}






void D3PartyJoinResponse::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeRangedUInt32(m_uResult, 0, 11, true);
    if (m_uResult == 0)
    {
        ptBitBuffer->writeRangedUInt32(m_atPeers.size(), 0, MAX_PEERS, true);
        for (const Peer & tPeer : m_atPeers)
        {
            tPeer.ptAddr->serialize(ptBitBuffer);
            ptBitBuffer->writeRangedUInt32(tPeer.uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
        }
    }
}



bdBool D3PartyJoinResponse::Deserialize(
    bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();


    bdBool bOk = ptBitBuffer->readRangedUInt32(m_uResult, 0, 11, true);
    if (m_uResult == 0)
    {
        uint32 uNumPeers = 0;
        bOk = bOk && ptBitBuffer->readRangedUInt32(uNumPeers, 0, MAX_PEERS, true);
        for (; bOk && uNumPeers > 0 && !m_atPeers.full(); uNumPeers--)
        {
            Peer & tPeer = m_atPeers.push_back();
            tPeer.ptAddr = ::new bdCommonAddr();

            bOk = tPeer.ptAddr->deserialize(me, ptBitBuffer);
            bOk = bOk && ptBitBuffer->readRangedUInt32(tPeer.uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
        }
    }

    return bOk;
}




D3PartyMsgPeerHello::D3PartyMsgPeerHello(bdSecurityID tSecID, bdSecurityKey tSecKey, bdNATType eNATType)
    : m_tSecID(tSecID),
      m_tSecKey(tSecKey),
      m_uNATType(eNATType)
{
}




void D3PartyMsgPeerHello::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeFull(m_tSecID.ab);
    ptBitBuffer->writeFull(m_tSecKey.ab);
    ptBitBuffer->writeRangedUInt32(GetNATType(), BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
}


bdBool D3PartyMsgPeerHello::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdBool bOk = ptBitBuffer->readFull(m_tSecID.ab) &&
                 ptBitBuffer->readFull(m_tSecKey.ab) &&
                 ptBitBuffer->readRangedUInt32(m_uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
    return bOk;
}





D3PartyMsgJoinParty::D3PartyMsgJoinParty(bdCommonAddrRef ptAddr, bdSecurityID tSecID, bdSecurityKey tSecKey)
    : m_ptAddr(ptAddr),
      m_tSecID(tSecID),
      m_tSecKey(tSecKey)
{
}




void D3PartyMsgJoinParty::Serialize(bdBitBufferRef ptBitBuffer) const
{
    m_ptAddr->serialize(ptBitBuffer);
    ptBitBuffer->writeFull(m_tSecID.ab);
    ptBitBuffer->writeFull(m_tSecKey.ab);
}


bdBool D3PartyMsgJoinParty::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    m_ptAddr = ::new bdCommonAddr();
    bdBool bOk = m_ptAddr->deserialize(me, ptBitBuffer);
    bOk = bOk &&
          ptBitBuffer->readFull(m_tSecID.ab) &&
          ptBitBuffer->readFull(m_tSecKey.ab);
    return bOk;
}






D3PartyMsgAddPlayerRequest::D3PartyMsgAddPlayerRequest(int32 nControllerIndex, const char * szPlayerName, AdditionalPlatformInformation tAdditionalPlatformInfo, OnlineUserId idOnlineUser, PlayerType ePlayerType, int32 nSponsorIndex, BOOL bVoiceActive, BOOL bBadReputation)
    : m_nControllerIndex(nControllerIndex),
      m_idOnlineUser(idOnlineUser),
      m_ePlayerType(ePlayerType),
      m_nGuestSponsor(nSponsorIndex),
      m_bVoiceActive(bVoiceActive != FALSE),
      m_bBadReputation(bBadReputation != FALSE)
{
    Blizzard::String::Copy(m_szPlayerName, szPlayerName, sizeof(m_szPlayerName));
    SigmaMemoryCopy(&m_tAdditionalPlatformInfo, &tAdditionalPlatformInfo, sizeof(m_tAdditionalPlatformInfo));
}











void D3PartyMsgAddPlayerRequest::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeRangedInt32(m_nControllerIndex, 0, 3);
    ptBitBuffer->writeString(bdString(m_szPlayerName));
    ptBitBuffer->writeFull(m_tAdditionalPlatformInfo);
    ptBitBuffer->writeFull(m_idOnlineUser);
    ptBitBuffer->writeRangedUInt32(m_ePlayerType, 0, 3, true);
    ptBitBuffer->writeRangedInt32(m_nGuestSponsor, -1, 3);
    ptBitBuffer->writeBool(m_bVoiceActive);
    ptBitBuffer->writeBool(m_bBadReputation);
}







bdBool D3PartyMsgAddPlayerRequest::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdBool bOk = ptBitBuffer->readRangedInt32(m_nControllerIndex, 0, 3) &&
                 ptBitBuffer->readString(m_szPlayerName, sizeof(m_szPlayerName)) &&
                 ptBitBuffer->readFull(m_tAdditionalPlatformInfo) &&
                 ptBitBuffer->readFull(m_idOnlineUser) &&
                 ptBitBuffer->readRangedUInt32(m_ePlayerType, 0, 3, true) &&
                 ptBitBuffer->readRangedInt32(m_nGuestSponsor, -1, 3) &&
                 ptBitBuffer->readBool(m_bVoiceActive) &&
                 ptBitBuffer->readBool(m_bBadReputation);
    return bOk;
}










D3PartyMsgAddPlayerResponse::D3PartyMsgAddPlayerResponse(bdInt32 nControllerIndex, bdByte8 idPlayer)
    : m_nControllerIndex(nControllerIndex),
      m_idPlayer(idPlayer)
{
}



void D3PartyMsgAddPlayerResponse::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeRangedInt32(m_nControllerIndex, 0, 3);
    ptBitBuffer->writeByte8(m_idPlayer);
}


bdBool D3PartyMsgAddPlayerResponse::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bool bOk = ptBitBuffer->readRangedInt32(m_nControllerIndex, 0, 3) &&
               m_nControllerIndex >= 0 && m_nControllerIndex < 4 &&
               ptBitBuffer->readByte8(m_idPlayer);
    return bOk;
}





D3PartyMsgRemovePlayer::D3PartyMsgRemovePlayer(int32 nControllerIndex)
    : m_nControllerIndex(nControllerIndex)
{
}


void D3PartyMsgRemovePlayer::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeRangedInt32(m_nControllerIndex, 0, 3);
}



bdBool D3PartyMsgRemovePlayer::Deserialize(bdBitBufferRef ptBitBuffer)
{
    return ptBitBuffer->readRangedInt32(m_nControllerIndex, 0, 3);
}





D3PartyMsgPlayerDataChunk::D3PartyMsgPlayerDataChunk(bdUByte8 idPlayer, PartyChunkedDataType eType, uint32 uUncompressedSize)
    : m_idPlayer(idPlayer),
      m_bComplete(false),
      m_eType(eType),
      m_uUncompressedSize(uUncompressedSize)
{
}




uint32 D3PartyMsgPlayerDataChunk::Serialize(bdBitBufferRef ptBitBuffer, const uint8 * ptData, uint32 uDataSize) const
{
    uint32 uWriteSize = Math::Min<uint32>(uDataSize, MAX_CHUNK_SIZE);
    ptBitBuffer->writeUByte8(m_idPlayer);
    ptBitBuffer->writeRangedUInt32(m_eType, CHUNKED_DATA_INVALID, CHUNKED_DATA_MAX, true);
    ptBitBuffer->writeUInt32(m_uUncompressedSize);
    ptBitBuffer->writeBool(uDataSize <= MAX_CHUNK_SIZE);
    ptBitBuffer->writeBits(ptData, uWriteSize * 8);
    return uWriteSize;
}









bdBool D3PartyMsgPlayerDataChunk::Deserialize(bdBitBufferRef ptBitBuffer, uint8 * ptData, uint32 & uDataSize)
{
    uint32 uType = CHUNKED_DATA_INVALID;
    bool bOk = ptBitBuffer->readUByte8(m_idPlayer) &&
               ptBitBuffer->readRangedUInt32(uType, CHUNKED_DATA_INVALID, CHUNKED_DATA_MAX, true) &&
               ptBitBuffer->readUInt32(m_uUncompressedSize) &&
               ptBitBuffer->readBool(m_bComplete);
    if (bOk)
    {
        uDataSize = (ptBitBuffer->getWritePosition() - ptBitBuffer->getReadPosition()) / 8;
        bOk = ptBitBuffer->readBits(ptData, uDataSize * 8);
    }

    m_eType = (PartyChunkedDataType)uType;

    return bOk && m_eType != CHUNKED_DATA_INVALID && m_idPlayer < 4;
}












D3PartyMsgMigrationRequest::D3PartyMsgMigrationRequest()
{
}

void D3PartyMsgMigrationRequest::Serialize(bdBitBufferRef ptBitBuffer)
{
    m_ptHostAddress->serialize(ptBitBuffer);
}


bdBool D3PartyMsgMigrationRequest::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    m_ptHostAddress = ::new bdCommonAddr();
    bdBool bOk = m_ptHostAddress->deserialize(me, ptBitBuffer);
    return bOk;
}







D3PartyMsgMigrationIAmHost::D3PartyMsgMigrationIAmHost()
{
}

void D3PartyMsgMigrationIAmHost::AddPeers(const blz::vector<PeerRef> & aptPeers)
{
    for (PeerRef ptPartyPeer : aptPeers)
    {
        Peer & tPeer = m_atPeers.push_back();
        tPeer.ptAddr = ptPartyPeer->m_ptConnection->getAddress();
        tPeer.uNATType = ptPartyPeer->m_eNATType;
    }
}





void D3PartyMsgMigrationIAmHost::AddSelf(const bdCommonAddrRef ptAddress, bdNATType eNATType)
{
    Peer & tPeer = m_atPeers.push_back();
    tPeer.ptAddr = ptAddress;
    tPeer.uNATType = eNATType;
}





void D3PartyMsgMigrationIAmHost::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeRangedUInt32(m_atPeers.size(), 0, MAX_PEERS, true);
    for (const Peer & tPeer : m_atPeers)
    {
        tPeer.ptAddr->serialize(ptBitBuffer);
        ptBitBuffer->writeRangedUInt32(tPeer.uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
    }
}


bdBool D3PartyMsgMigrationIAmHost::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    uint32 uNumPeers = 0;
    bdBool bOk = ptBitBuffer->readRangedUInt32(uNumPeers, 0, MAX_PEERS, true);
    for (; bOk && uNumPeers > 0 && !m_atPeers.full(); uNumPeers--)
    {
        Peer & tPeer = m_atPeers.push_back();
        tPeer.ptAddr = ::new bdCommonAddr();
        bOk = tPeer.ptAddr->deserialize(me, ptBitBuffer);
        bOk = bOk && ptBitBuffer->readRangedUInt32(tPeer.uNATType, BD_NAT_UNKNOWN, BD_NAT_STRICT, true);
    }

    return bOk;
}







D3PartyMsgUpdatePlayers::D3PartyMsgUpdatePlayers()
    : m_uNumPlayers(0)
{
}































bdBool D3PartyMsgUpdatePlayers::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdBool bOk = ptBitBuffer->readUByte8(m_uNumPlayers);
    for (uint32 i = 0; bOk && i < m_uNumPlayers; i++)
    {
        PlayerInfo & tInfo = m_tPlayerInfo[i];
        bOk = ptBitBuffer->readRangedInt32(tInfo.m_nControllerIndex, 0, 3) &&
              ptBitBuffer->readString(tInfo.m_szPlayerName, sizeof(tInfo.m_szPlayerName)) &&
              ptBitBuffer->readFull(tInfo.m_idOnlineUser) &&
              ptBitBuffer->readBool(tInfo.m_bBadReputation);
    }
    return bOk;
}







D3PartyMsgPlayerInGame::D3PartyMsgPlayerInGame(uint8 idPlayer, BOOL bInGame)
    : m_idPlayer(idPlayer),
      m_bInGame(bInGame != FALSE)
{
}



void D3PartyMsgPlayerInGame::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeUByte8(m_idPlayer);
    ptBitBuffer->writeBool(m_bInGame);
}


bdBool D3PartyMsgPlayerInGame::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdBool bOk = ptBitBuffer->readUByte8(m_idPlayer) &&
                 ptBitBuffer->readBool(m_bInGame);
    return bOk;
}

}  // namespace Online
}  // namespace Console
