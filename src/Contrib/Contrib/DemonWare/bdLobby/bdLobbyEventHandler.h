// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyEventHandler.h

#pragma once

#include "bdAntiCheat/bdAntiCheat.h"
#include "bdAntiCheat/bdAntiCheatChallenges.h"
#include "bdCODO/bdCODO.h"
#include "bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include "bdRichPresence/bdRichPresenceData.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAchievementsUpdatedPushMessage;
class bdChannelInfo;
class bdChannelMember;
class bdFeatureBanInfo;
class bdMarketplaceCouponInventory;
class bdMarketplaceCouponInventoryV2;
class bdMarketplaceCouponInventoryV3;
class bdMarketplaceCouponInventoryV4;
class bdMarketplaceCurrency;
class bdMarketplaceInventory;
class bdRewardEventJSON;
class bdRewardEventJSONV2;
class bdSessionID;
class bdSessionInvite;
class bdTSSStructMessage;
class bdTencentAASRecord;
class bdUserAccountID;

class bdLobbyEventHandler {  // size 0x8
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdLobbyEventHandler);
    bdLobbyEventHandler();
    virtual ~bdLobbyEventHandler();  // vtable slot 0
    virtual void onPlayerStatusUpdate(bdOnlineUserID, bdNChar8 *, bdUByte8);  // vtable slot 2
    virtual void onTeamMemberStatusUpdate(bdOnlineUserID, bdNChar8 *, bdTeamID, bdNChar8 *, bdUByte8);  // vtable slot 3
    virtual void onRichPresenceUpdate(bdOnlineUserID, bdNChar8 *, void *, bdUInt, bdTeamID);  // vtable slot 4
    virtual void onChatChannelUpdate(bdUInt64, bdOnlineUserID, bdNChar8 *, bdUByte8);  // vtable slot 5
    virtual void onChatChannelUpdate(const bdChannelInfo & channel, const bdChannelMember & member, bdUByte8 status);  // vtable slot 6
    virtual void onChatChannelMessage(bdUInt64, bdOnlineUserID, bdNChar8 *, bdUInt32, void *, bdUInt);  // vtable slot 7
    virtual void onChatChannelUserPromoted(const bdChannelInfo &, const bdChannelMember &, const bdChannelMember &);  // vtable slot 8
    virtual void onChatChannelUserKicked(const bdChannelInfo &, const bdChannelMember &, const bdChannelMember &, bdUInt32, bdBool);  // vtable slot 9
    virtual void onInstantMessage(bdOnlineUserID, bdNChar8 *, void *, bdUInt, bdTeamID);  // vtable slot 10
    virtual void onSessionInvite(bdSessionInvite &);  // vtable slot 11
    virtual void onNotifyLeave(bdOnlineUserID, bdSessionID);  // vtable slot 12
    virtual void onFriendshipProposal();  // vtable slot 13
    virtual void onTeamProposal();  // vtable slot 14
    virtual void onNewNotification();  // vtable slot 15
    virtual void onNewMail();  // vtable slot 16
    virtual void onMultipleLogon(bdOnlineUserID);  // vtable slot 17
    virtual void onChallengesReceived(bdOnlineUserID, bdAntiCheatChallengesRef);  // vtable slot 18
    virtual void onPlayerDisconnected(bdOnlineUserID, bdAntiCheat::bdAntiCheatReason);  // vtable slot 19
    virtual void onPlayerBanned(bdOnlineUserID, bdUInt32, bdInt64, bdUInt32);  // vtable slot 20
    virtual void onConsoleBanned(bdOnlineUserID);  // vtable slot 21
    virtual void onNotWhiteListed(bdOnlineUserID);  // vtable slot 22
    virtual void onNotWhiteListedWithMessage(bdOnlineUserID, bdNChar8 *);  // vtable slot 23
    virtual void onTOTPMessage(bdOnlineUserID);  // vtable slot 24
    virtual void onMultipleLinkedAccountLogons(bdUserAccountID);  // vtable slot 25
    virtual void onLinkedAccountStatusChange(bdUserAccountID);  // vtable slot 26
    virtual void onGlobalInstantMessage(bdOnlineUserID, bdNChar8 *, void *, bdUInt);  // vtable slot 27
    virtual void onFeatureBan(const bdFeatureBanInfo &);  // vtable slot 28
    virtual void onGroupMessagingMessage(bdOnlineUserID, const bdNChar8 *, void *, bdUInt);  // vtable slot 29
    virtual void onBroadcastMessage(bdOnlineUserID, const bdNChar8 *, void *, bdUInt);  // vtable slot 30
    virtual void onTencentAASRecordReceived(const bdTencentAASRecord &);  // vtable slot 31
    virtual void onTencentAntibotData(const bdNChar8 *, bdUInt32);  // vtable slot 32
    virtual void onTencentAntibotPunish(bdUInt32, bdUInt32, const bdNChar8 *, bdUInt32, const bdNChar8 *, bdUInt32);  // vtable slot 33
    virtual void onTencentAntibotServerReady();  // vtable slot 34
    virtual void onTencentNotifyLeaveReason(bdOnlineUserID, bdSessionID, bdUInt32);  // vtable slot 35
    virtual void onTencentLoudSpeakerMessage(bdOnlineUserID, bdNChar8 *, bdCODOLoudSpeakerType, void *, bdUInt);  // vtable slot 36
    virtual void onPlayerMuted(bdOnlineUserID, bdUInt64);  // vtable slot 37
    virtual void onPlayerStabilised(bdOnlineUserID, bdUInt64);  // vtable slot 38
    virtual void onTencentPlayerReward(bdOnlineUserID, bdUInt32, const bdNChar8 *, bdUInt32, const bdUByte8 *);  // vtable slot 39
    virtual void onTencentPlayerNoReward(bdOnlineUserID, bdUInt32, const bdNChar8 *);  // vtable slot 40
    virtual void onTSSResponseReceived(const bdTSSStructMessage &);  // vtable slot 41
    virtual void onTencentUsernameChanged(const bdNChar8 *, const bdUInt32);  // vtable slot 42
    virtual void onMarketplaceCouponsGranted(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, bdNChar8 (*)[25]);  // vtable slot 43
    virtual void onMarketplaceItemsExpired(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, bdUInt32 *);  // vtable slot 44
    virtual void onInventoryItemUpdate(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceInventory *);  // vtable slot 45
    virtual void onInventoryCouponUpdate(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventory *);  // vtable slot 46
    virtual void onInventoryCouponUpdateV2(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV2 *);  // vtable slot 47
    virtual void onInventoryCouponUpdateV3(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV3 *);  // vtable slot 48
    virtual void onInventoryCouponUpdateV4(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdMarketplaceCouponInventoryV4 *);  // vtable slot 49
    virtual void onBalanceUpdate(const bdUserAccountID, const bdUInt32, const bdMarketplaceCurrency *);  // vtable slot 50
    virtual void onClaimAchievement(bdUserAccountID, bdNChar8 *, bdUInt32, bdUInt32, const bdMarketplaceInventory *);  // vtable slot 51
    virtual void onDepositGranted(const bdUserAccountID, const bdUInt32, const bdMarketplaceCurrency *);  // vtable slot 52
    virtual void onItemsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *);  // vtable slot 53
    virtual void onItemsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *, const bdUInt32 *);  // vtable slot 54
    virtual void onEntitlementsGranted(const bdUserAccountID, const bdNChar8 *, const bdUInt32, const bdUInt32 *);  // vtable slot 55
    virtual void onWebserviceStorageFileWrite(const bdNChar8 *, const bdNChar8 *);  // vtable slot 56
    virtual void onRewardEventJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSON &);  // vtable slot 57
    virtual void onRewardEventJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSONV2 &);  // vtable slot 58
    virtual void onRewardAchievementJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSON &);  // vtable slot 59
    virtual void onRewardAchievementJSONMessage(const bdUserAccountID, const bdNChar8 *, const bdRewardEventJSONV2 &);  // vtable slot 60
    virtual void onMarketingCommsAssignmentsAvailable();  // vtable slot 61
    virtual void onPublisherVariablesUpdate(const bdNChar8 *, const bdNChar8 *, const bdUInt16, const bdUInt16, const bdBool);  // vtable slot 62
    virtual void onUserPrivateProfileUpdated();  // vtable slot 63
    virtual void onTeamMemberRichPresenceUpdate(bdOnlineUserID, void *, bdUInt, bdBool);  // vtable slot 64
    virtual void onTeamMemberUserNameUpdate(bdOnlineUserID, const bdNChar8 *);  // vtable slot 65
    virtual void onTitleVersionDisabled(bdOnlineUserID);  // vtable slot 66
    virtual void onUserPrivateProfileUpdatedWithCallerInfo(bdBool);  // vtable slot 67
    virtual void onCODOTeamMarketplaceLevelChange(bdUInt64, bdUInt32, bdUInt32);  // vtable slot 68
    virtual void onSubscribedRichPresenceUpdate(const bdNChar8 *, const bdTitleID, const bdRichPresenceSource, const bdRichPresenceData &);  // vtable slot 69
    virtual void onLogonTimeProhibited(bdUByte8 *, bdUInt32);  // vtable slot 70
    virtual void onLogonTimeProhibitedWarning(bdUInt32, bdUByte8 *, bdUInt32);  // vtable slot 71
    virtual void onAchievementsUpdated(const bdAchievementsUpdatedPushMessage & pushMessage);  // vtable slot 72
};
static_assert(sizeof(bdLobbyEventHandler) == 0x8, "bdLobbyEventHandler");


// Functions defined in this file (bodies are not in the DWARF):
//   line    44  void bdLobbyEventHandler::operator delete(void * p)  [inlined into 2 functions]
