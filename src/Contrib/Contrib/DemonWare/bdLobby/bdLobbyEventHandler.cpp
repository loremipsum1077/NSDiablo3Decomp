// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyEventHandler.cpp
#include "bdLobbyEventHandler.h"
#include "bdMessaging/bdChannelInfo.h"
#include "bdMessaging/bdChannelMember.h"
#include "bdMatchMaking/bdSessionID.h"
#include "../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"






bdLobbyEventHandler::bdLobbyEventHandler()
{
}

bdLobbyEventHandler::~bdLobbyEventHandler()
{
}

void bdLobbyEventHandler::onPlayerStatusUpdate(bdOnlineUserID, bdNChar8 *, bdUByte8)
{
}




void bdLobbyEventHandler::onTeamMemberStatusUpdate(bdOnlineUserID, bdNChar8 *, bdTeamID, bdNChar8 *, bdUByte8)
{
}





void bdLobbyEventHandler::onRichPresenceUpdate(bdOnlineUserID, bdNChar8 *, void *, bdUInt, bdTeamID)
{
}





void bdLobbyEventHandler::onChatChannelUpdate(bdUInt64, bdOnlineUserID, bdNChar8 *, bdUByte8)
{
}




void bdLobbyEventHandler::onChatChannelUpdate(const bdChannelInfo & channel, const bdChannelMember & member, bdUByte8 status)
{


    bdNChar8 constCaster[64];

    bdMemcpy(constCaster, member.m_userName, sizeof(constCaster));

    onChatChannelUpdate(channel.m_channelID, member.m_userID, constCaster, status);
}

void bdLobbyEventHandler::onChatChannelMessage(bdUInt64, bdOnlineUserID, bdNChar8 *, bdUInt32, void *, bdUInt)
{
}






void bdLobbyEventHandler::onChatChannelUserPromoted(const bdChannelInfo &, const bdChannelMember &, const bdChannelMember &)
{
}



void bdLobbyEventHandler::onChatChannelUserKicked(const bdChannelInfo &, const bdChannelMember &, const bdChannelMember &, bdUInt32, bdBool)
{
}





void bdLobbyEventHandler::onInstantMessage(bdOnlineUserID, bdNChar8 *, void *, bdUInt, bdTeamID)
{
}





void bdLobbyEventHandler::onSessionInvite(bdSessionInvite &)
{
}

void bdLobbyEventHandler::onFriendshipProposal()
{
}

void bdLobbyEventHandler::onTeamProposal()
{
}

void bdLobbyEventHandler::onNewNotification()
{
}

void bdLobbyEventHandler::onNewMail()
{
}

void bdLobbyEventHandler::onNotifyLeave(bdOnlineUserID, bdSessionID)
{
}


void bdLobbyEventHandler::onChallengesReceived(bdOnlineUserID, bdAntiCheatChallengesRef)
{
}


void bdLobbyEventHandler::onMultipleLogon(bdOnlineUserID)
{
}


void bdLobbyEventHandler::onPlayerDisconnected(bdOnlineUserID, bdAntiCheat::bdAntiCheatReason)
{
}


void bdLobbyEventHandler::onPlayerBanned(bdOnlineUserID, bdUInt32, bdInt64, bdUInt32)
{
}




void bdLobbyEventHandler::onConsoleBanned(bdOnlineUserID)
{
}

void bdLobbyEventHandler::onMultipleLinkedAccountLogons(bdUserAccountID)
{
}


void bdLobbyEventHandler::onLinkedAccountStatusChange(bdUserAccountID)
{
}

void bdLobbyEventHandler::onNotWhiteListed(bdOnlineUserID)
{
}

void bdLobbyEventHandler::onNotWhiteListedWithMessage(bdOnlineUserID, bdNChar8 *)
{
}


void bdLobbyEventHandler::onTOTPMessage(bdOnlineUserID)
{
}

void bdLobbyEventHandler::onGlobalInstantMessage(bdOnlineUserID, bdNChar8 *, void *, bdUInt)
{
}




void bdLobbyEventHandler::onFeatureBan(const bdFeatureBanInfo &)
{
}

void bdLobbyEventHandler::onGroupMessagingMessage(bdOnlineUserID, const bdNChar8 *, void *, bdUInt)
{
}




void bdLobbyEventHandler::onBroadcastMessage(bdOnlineUserID, const bdNChar8 *, void *, bdUInt)
{
}




void bdLobbyEventHandler::onTencentAASRecordReceived(const bdTencentAASRecord &)
{
}

void bdLobbyEventHandler::onTencentAntibotData(const bdNChar8 *, bdUInt32)
{
}


void bdLobbyEventHandler::onTencentAntibotPunish(bdUInt32, bdUInt32, const bdNChar8 *, bdUInt32, const bdNChar8 *, bdUInt32)
{
}






void bdLobbyEventHandler::onTencentAntibotServerReady()
{
}


void bdLobbyEventHandler::onTencentNotifyLeaveReason(bdOnlineUserID, bdSessionID, bdUInt32)
{
}



void bdLobbyEventHandler::onTencentLoudSpeakerMessage(bdOnlineUserID, bdNChar8 *, bdCODOLoudSpeakerType, void *, bdUInt)
{
}






void bdLobbyEventHandler::onPlayerMuted(bdOnlineUserID, bdUInt64)
{
}


void bdLobbyEventHandler::onPlayerStabilised(bdOnlineUserID, bdUInt64)
{
}


void bdLobbyEventHandler::onTencentPlayerReward(bdOnlineUserID, bdUInt32, const bdNChar8 *, bdUInt32, const bdUByte8 *)
{
}





void bdLobbyEventHandler::onTencentPlayerNoReward(bdOnlineUserID, bdUInt32, const bdNChar8 *)
{
}



void bdLobbyEventHandler::onTSSResponseReceived(const bdTSSStructMessage &)
{
}

void bdLobbyEventHandler::onTencentUsernameChanged(const bdNChar8 *, const bdUInt32)
{
}


void bdLobbyEventHandler::onMarketplaceCouponsGranted(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, bdNChar8 (*)[25])
{
}





void bdLobbyEventHandler::onMarketplaceItemsExpired(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, bdUInt32 *)
{
}





void bdLobbyEventHandler::onInventoryItemUpdate(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceInventory *)
{
}




void bdLobbyEventHandler::onInventoryCouponUpdate(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventory *)
{
}




void bdLobbyEventHandler::onInventoryCouponUpdateV2(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV2 *)
{
}




void bdLobbyEventHandler::onInventoryCouponUpdateV3(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV3 *)
{
}




void bdLobbyEventHandler::onInventoryCouponUpdateV4(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV4 *)
{
}




void bdLobbyEventHandler::onBalanceUpdate(const bdUserAccountID, const bdUInt32, const bdMarketplaceCurrency *)
{
}



void bdLobbyEventHandler::onClaimAchievement(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, const bdMarketplaceInventory *)
{
}





void bdLobbyEventHandler::onDepositGranted(const bdUserAccountID, const bdUInt32, const bdMarketplaceCurrency *)
{
}



void bdLobbyEventHandler::onItemsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *)
{
}




void bdLobbyEventHandler::onItemsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *, const bdUInt32 *)
{
}





void bdLobbyEventHandler::onEntitlementsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *)
{
}



void bdLobbyEventHandler::onWebserviceStorageFileWrite(const bdNChar8 *, const bdNChar8 *)
{
}


void bdLobbyEventHandler::onRewardEventJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSON &)
{
}





void bdLobbyEventHandler::onRewardEventJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSONV2 &)
{
}



void bdLobbyEventHandler::onRewardAchievementJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSON &)
{
}





void bdLobbyEventHandler::onRewardAchievementJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSONV2 &)
{
}



void bdLobbyEventHandler::onMarketingCommsAssignmentsAvailable()
{
}

void bdLobbyEventHandler::onPublisherVariablesUpdate(const bdNChar8 *, const bdNChar8 *, const bdUInt16, const bdUInt16, const bdBool)
{
}





void bdLobbyEventHandler::onUserPrivateProfileUpdated()
{
}

void bdLobbyEventHandler::onTeamMemberRichPresenceUpdate(bdOnlineUserID, void *, bdUInt, bdBool)
{
}




void bdLobbyEventHandler::onTeamMemberUserNameUpdate(bdOnlineUserID, const bdNChar8 *)
{
}


void bdLobbyEventHandler::onTitleVersionDisabled(bdOnlineUserID)
{
}

void bdLobbyEventHandler::onUserPrivateProfileUpdatedWithCallerInfo(bdBool)
{
}

void bdLobbyEventHandler::onCODOTeamMarketplaceLevelChange(bdUInt64, bdUInt32, bdUInt32)
{
}



void bdLobbyEventHandler::onSubscribedRichPresenceUpdate(const bdNChar8 *, const bdTitleID, const bdRichPresenceSource, const bdRichPresenceData &)
{
}




void bdLobbyEventHandler::onLogonTimeProhibited(bdUByte8 *, bdUInt32)
{
}


void bdLobbyEventHandler::onLogonTimeProhibitedWarning(bdUInt32, bdUByte8 *, bdUInt32)
{
}



void bdLobbyEventHandler::onAchievementsUpdated(const bdAchievementsUpdatedPushMessage & pushMessage)
{
}
